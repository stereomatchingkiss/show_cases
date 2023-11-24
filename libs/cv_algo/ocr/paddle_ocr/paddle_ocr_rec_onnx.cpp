#include "paddle_ocr_rec_onnx.hpp"

#include "crnn_process.hpp"

#ifndef WASM_BUILD

#include "../../onnx/onnx_get_names_utils.hpp"
#include <onnxruntime_cxx_api.h>

#else

#include <emscripten.h>

EM_JS(void, js_rec_text, (float *img, int width, int height), {
    recOcrText(img, width, height);
})

EM_JS(int, js_get_global_candidate_text_size, (), {
    return getGlobalCandidateTextSize();
})

EM_JS(float*, js_get_global_buffer, (), {
    return getGlobalBuffer();
})

EM_JS(int, js_get_global_buffer_available, (), {
    return getGlobalBufferAvailable();
})

EM_JS(void, js_release_global_buffer, (), {
    releaseGlobalBuffer();
})

EM_JS(void, js_create_global_session, (), {
    createGlobalSession();
})

#endif

#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace flt::cvt::ocr{

namespace{

std::vector<std::string> read_keys(std::string const &fname)
{
    std::vector<std::string> keys_vec;
    if(std::ifstream in(fname); in.is_open()){
        std::string line;
        while(getline(in, line)){
            keys_vec.emplace_back(line);
        }
    }else{
        std::cout<<"cannot open fname = "<<fname<<std::endl;
    }
    
    return keys_vec;
}

}

struct paddle_ocr_rec_onnx::impl
{
    impl(std::string const &model_weights, std::string const &key_files, int dst_height, int max_width) :
        dist_height_{dst_height},
        keys_{read_keys(key_files)},
        max_width_{max_width}
    {
#ifndef WASM_BUILD
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "paddle_ocr_rec_onnx");
        
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
        
        std::wstring mname(std::begin(model_weights), std::end(model_weights));
        session_ = std::make_unique<Ort::Session>(env, mname.c_str(), session_options);
        
        onnx_utils_.process(*session_);        
        onnx_utils_.input_node_dims()[0] = 1;
        onnx_utils_.input_node_dims()[2] = dist_height_;
        
        onnx_utils_.print_model_details();
#endif

#ifdef WASM_BUILD
        js_create_global_session();        
#endif
        
        keys_.insert(keys_.begin(), "#"); // blank char for ctc
        keys_.push_back(" ");        
    }
    
    void normalize(cv::Mat const &img)
    {
        int const row = img.rows;
        int const col = img.cols;
        std::vector<float> input_image;
        input_data_.resize(img.total() * img.channels());
        
        for(int c = 0; c < 3; c++){
            for (int i = 0; i < row; i++){
                for (int j = 0; j < col; j++){
                    float pix = img.ptr<uchar>(i)[j * 3 + c];
                    input_data_[c * row * col + i * col + j] = (pix / 255.0f - 0.5f) / 0.5f;
                }
            }
        }        
    }

#ifndef WASM_BUILD
    auto create_input_tensor()
    {        
        return Ort::Value::CreateTensor<float>(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault),
                                               input_data_.data(),
                                               input_data_.size(),
                                               onnx_utils_.input_node_dims().data(),
                                               4);
    }
    
    auto create_output_tensor(Ort::Value const &input_tensor)
    {
        return  session_->Run(Ort::RunOptions{nullptr},
                             onnx_utils_.input_node_names().data(),
                             &input_tensor,
                             1,
                             onnx_utils_.output_node_names().data(),
                             1);
    }
#endif
    
    void predict(cv::Mat const &mat, std::vector<TextBox> &boxes)
    {
        for(size_t i = 0; i != boxes.size(); ++i){
            
            auto const im_size = preprocess(mat, boxes[i].boxPoint);

#ifndef WASM_BUILD
            onnx_utils_.input_node_dims()[3] = im_size.width;
            
            auto input_tensor = create_input_tensor();
            auto output_tensors = create_output_tensor(input_tensor);
            
            float const *floatarr = output_tensors.front().GetTensorData<float>();
            auto const outputInfo = output_tensors.front().GetTensorTypeAndShapeInfo();            
            
            size_t const elem_size = outputInfo.GetShape()[2];
            size_t const candidate = outputInfo.GetShape()[1];

            process_rec_results(boxes[i], floatarr, candidate, elem_size);
#endif
        }
    }    

    cv::Size preprocess(cv::Mat const &mat, std::vector<cv::Point> const &box_points)
    {
        auto crop_img = get_rotate_crop_image(mat, box_points);
        float const wh_ratio = static_cast<float>(crop_img.cols) / static_cast<float>(crop_img.rows);
        auto resize_img = crnn_resize_img(crop_img, wh_ratio, dist_height_, max_width_);
        normalize(resize_img);

        return cv::Size(resize_img.cols, resize_img.rows);
    }

#ifdef WASM_BUILD
    void predict(TextBox &tbox)
    {
        float const *floatarr = js_get_global_buffer();
        int const candidate = js_get_global_candidate_text_size();

        process_rec_results(tbox, floatarr, candidate);
        releaseGlobalBuffer();
    }

    void async_predict(cv::Mat const &mat, TextBox const &text_boxes)
    {
        auto const im_size = preprocess(mat, text_boxes.boxPoint);

        js_rec_text(&input_data_[0], im_size.width, im_size.height);
    }
#endif
    
    void process_rec_results(TextBox &tbox, float const *floatarr, size_t candidate, size_t elem_size = 6625)
    {
        size_t last_index = 0;
        int count = 0;
        for(size_t n = 0; n < candidate; ++n){
            auto *data = &floatarr[n * elem_size];
            size_t argmax_idx = 0;
            float max_score = 0;
            for(size_t j = 0; j != elem_size; ++j){
                if(data[j] > max_score){
                    max_score = data[j];
                    argmax_idx = j;
                }
            }
            if (argmax_idx > 0 && (!(n > 0 && argmax_idx == last_index))) {
                tbox.score += max_score;
                count += 1;
                tbox.text += keys_[argmax_idx];                
            }
            last_index = argmax_idx;
        }
        
        if(count != 0){
            tbox.score /= count;
        }        
    }
    
    int dist_height_;
    std::vector<std::string> keys_;
    std::vector<float> input_data_;
    int max_width_;

#ifndef WASM_BUILD
    onnx_get_names_utils onnx_utils_;
    std::unique_ptr<Ort::Session> session_;
#endif
};

paddle_ocr_rec_onnx::paddle_ocr_rec_onnx(std::string const &model_weights,
                                         std::string const &key_files,
                                         int dst_height,
                                         int max_width) :
    impl_{std::make_unique<impl>(model_weights, key_files, dst_height, max_width)}
{
    
}

paddle_ocr_rec_onnx::~paddle_ocr_rec_onnx()
{
    
}

#ifdef WASM_BUILD

void paddle_ocr_rec_onnx::predict(TextBox &text_boxes)
{
    impl_->predict(text_boxes);
}

bool paddle_ocr_rec_onnx::predict_results_available() const
{
    return js_get_global_buffer_available();
}

void paddle_ocr_rec_onnx::async_predict(cv::Mat const &mat, TextBox const &text_boxes)
{
    impl_->async_predict(mat, text_boxes);
}

#else

void paddle_ocr_rec_onnx::predict(cv::Mat const &mat, std::vector<TextBox> &boxes)
{
    impl_->predict(mat, boxes);
}


#endif

}
