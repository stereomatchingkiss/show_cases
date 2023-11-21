#include "paddle_ocr_rec_onnx.hpp"

#include "crnn_process.hpp"

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

        get_input_names();
        get_output_names();//*/

        keys_.insert(keys_.begin(), "#"); // blank char for ctc
        keys_.push_back(" ");
        //std::cout<<"keys size = "<<keys_.size()<<std::endl;

        print_model_details();

        input_node_dims_[0] = 1;
        input_node_dims_[2] = dist_height_;
    }

    void print_model_details()
    {
        for(size_t j = 0; j < input_node_dims_.size(); ++j){
            std::cout << "Input " << j << " : dim[" << j << "] =" << input_node_dims_[j] << '\n';
        }
        for(size_t j = 0; j != input_node_names_.size(); ++j){
            std::cout<<"input names "<< j <<":"<<input_node_names_[j]<<"\n";
        }

        for(size_t j = 0; j < output_node_dims_.size(); ++j){
            std::cout << "Output " << j << " : dim[" << j << "] =" << output_node_dims_[j] << '\n';
        }
        for(size_t j = 0; j != output_node_names_.size(); ++j){
            std::cout<<"output names "<< j <<":"<<output_node_names_[j]<<"\n";
        }
        std::cout << std::flush;
    }

    void get_names(size_t node_index,
                   std::vector<int64_t> &node_dims,
                   std::vector<Ort::AllocatedStringPtr> &names_ptr,
                   std::vector<const char*> &node_names,
                   bool input) const
    {
        size_t const node_counts = input ? session_->GetInputCount() : session_->GetOutputCount();
        names_ptr.reserve(node_counts);
        node_names.reserve(node_counts);

        Ort::AllocatorWithDefaultOptions allocator;
        auto node_name = input ? session_->GetInputNameAllocated(node_index, allocator) :
                             session_->GetOutputNameAllocated(node_index, allocator);
        node_names.push_back(node_name.get());
        names_ptr.push_back(std::move(node_name));

        auto const type_info = session_->GetInputTypeInfo(node_index);
        auto const tensor_info = type_info.GetTensorTypeAndShapeInfo();

        node_dims = tensor_info.GetShape();
    }

    void get_input_names()
    {
        get_names(0, input_node_dims_, input_names_ptr_, input_node_names_, true);
    }

    void get_output_names()
    {
        get_names(0, output_node_dims_, output_names_ptr_, output_node_names_, false);
    }

    std::vector<float> normalize(cv::Mat const &img)
    {
        int const row = img.rows;
        int const col = img.cols;
        std::vector<float> input_image;
        input_image.resize(img.total() * img.channels());

        for(int c = 0; c < 3; c++){
            for (int i = 0; i < row; i++){
                for (int j = 0; j < col; j++){
                    if(j < col){
                        float pix = img.ptr<uchar>(i)[j * 3 + c];
                        input_image[c * row * col + i * col + j] = (pix / 255.0 - 0.5) / 0.5;
                    }
                }
            }
        }

        return input_image;
    }

    std::string predict(cv::Mat const &mat, std::vector<TextBox> const &boxes)
    {
        for(size_t i = 0; i != boxes.size(); ++i){

            auto crop_img = GetRotateCropImage(mat, boxes[i].boxPoint);
            float const wh_ratio = static_cast<float>(crop_img.cols) / static_cast<float>(crop_img.rows);
            auto resize_img = CrnnResizeImg(crop_img, wh_ratio, dist_height_);
            auto input_data = normalize(resize_img);

            input_node_dims_[3] = resize_img.cols;            
            auto const minfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);            
            auto input_tensor = Ort::Value::CreateTensor<float>(minfo,
                                                                input_data.data(),
                                                                input_data.size(),
                                                                input_node_dims_.data(),
                                                                4);


            auto output_tensors =
                session_->Run(Ort::RunOptions{nullptr}, input_node_names_.data(), &input_tensor, 1, output_node_names_.data(), 1);            

            float const *floatarr = output_tensors.front().GetTensorData<float>();
            auto outputInfo = output_tensors.front().GetTensorTypeAndShapeInfo();

            std::string str_res;
            int last_index = 0;
            float score = 0.f;
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
                    score += max_score;
                    count += 1;
                    str_res += keys_[argmax_idx];
                }
                last_index = argmax_idx;
            }

            std::cout<<"count = "<<count<<","<<str_res<<","<<score<<std::endl;//*/
        }

        return {};
    }

    int dist_height_;
    std::vector<std::string> keys_;

    std::vector<int64_t> input_node_dims_;
    std::vector<Ort::AllocatedStringPtr> input_names_ptr_;
    std::vector<const char*> input_node_names_;

    std::vector<int64_t> output_node_dims_;
    std::vector<Ort::AllocatedStringPtr> output_names_ptr_;
    std::vector<const char*> output_node_names_;

    std::unique_ptr<Ort::Session> session_;
};

paddle_ocr_rec_onnx::paddle_ocr_rec_onnx(std::string const &model_weights, std::string const &key_files, int dst_height) :
    impl_{std::make_unique<impl>(model_weights, key_files, dst_height)}
{

}

paddle_ocr_rec_onnx::~paddle_ocr_rec_onnx()
{

}

std::string paddle_ocr_rec_onnx::predict(cv::Mat const &mat, std::vector<TextBox> const &boxes)
{
    return impl_->predict(mat, boxes);
}

}
