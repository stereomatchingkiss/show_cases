#include "paddle_ocr_rec_onnx.hpp"

#include "crnn_process.hpp"
#include "../../onnx/onnx_get_names_utils.hpp"

#include <onnxruntime_cxx_api.h>

#include <format>
#include <fstream>
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
    }

    return keys_vec;
}

}

struct paddle_ocr_rec_onnx::impl
{
    impl(std::string const &model_weights, std::string const &key_files, int dst_height) :
        dist_height_{dst_height},
        keys_{read_keys(key_files)}
    {
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "paddle_ocr_rec_onnx");

        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

        std::wstring mname(std::begin(model_weights), std::end(model_weights));
        session_ = std::make_unique<Ort::Session>(env, mname.c_str(), session_options);

        onnx_utils_.process(*session_);        
        onnx_utils_.input_node_dims()[0] = 1;
        onnx_utils_.input_node_dims()[2] = dist_height_;

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

    void predict(cv::Mat const &mat, std::vector<TextBox> &boxes)
    {
        for(size_t i = 0; i != boxes.size(); ++i){

            auto crop_img = get_rotate_crop_image(mat, boxes[i].boxPoint);
            float const wh_ratio = static_cast<float>(crop_img.cols) / static_cast<float>(crop_img.rows);
            auto resize_img = crnn_resize_img(crop_img, wh_ratio, dist_height_);
            normalize(resize_img);

            onnx_utils_.input_node_dims()[3] = resize_img.cols;

            auto input_tensor = create_input_tensor();
            auto output_tensors = create_output_tensor(input_tensor);

            float const *floatarr = output_tensors.front().GetTensorData<float>();
            auto const outputInfo = output_tensors.front().GetTensorTypeAndShapeInfo();

            auto &tbox = boxes[i];
            size_t last_index = 0;
            int count = 0;
            size_t const elem_size = outputInfo.GetShape()[2];
            size_t const candidate = outputInfo.GetShape()[1];
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
    }

    int dist_height_;
    std::vector<std::string> keys_;
    std::vector<float> input_data_;
    onnx_get_names_utils onnx_utils_;
    std::unique_ptr<Ort::Session> session_;
};

paddle_ocr_rec_onnx::paddle_ocr_rec_onnx(std::string const &model_weights, std::string const &key_files, int dst_height) :
    impl_{std::make_unique<impl>(model_weights, key_files, dst_height)}
{

}

paddle_ocr_rec_onnx::~paddle_ocr_rec_onnx()
{

}

void paddle_ocr_rec_onnx::predict(cv::Mat const &mat, std::vector<TextBox> &boxes)
{
    impl_->predict(mat, boxes);
}

}
