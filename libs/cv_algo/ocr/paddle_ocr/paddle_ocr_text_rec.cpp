#include "paddle_ocr_text_rec.hpp"

#include <fstream>
#include <iterator>

#include <net.h>

namespace flt::cvt::ocr{

namespace{

inline static size_t arg_max(std::forward_iterator auto first, std::forward_iterator auto last) {
    return std::distance(first, std::max_element(first, last));
}

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

struct paddle_ocr_text_rec::impl
{
    impl(const char *param, const char *bin, const char *key_files, int dst_height, bool use_gpu, int max_thread) :
        dst_height_{dst_height},
        keys_{read_keys(key_files)}
    {
        has_gpu_ = ncnn::get_gpu_count() > 0 && use_gpu;
        net_.opt.use_vulkan_compute = has_gpu_;
        net_.opt.use_fp16_arithmetic = true;
        net_.opt.num_threads = max_thread_;
        load_param_success_ = net_.load_param(param);
        load_model_success_ = net_.load_model(bin);

        input_name_ = net_.input_names()[0];
        output_name_ = net_.output_names()[net_.output_names().size() - 1];
    }

    TextLine score_to_text_line(float *outputData, int h, int w) const
    {
        size_t const keySize = keys_.size();
        std::string strRes;
        std::vector<float> scores;
        int lastIndex = 0;

        for(int i = 0; i < h; i++)
        {
            int const maxIndex = static_cast<int>(arg_max(outputData + i*w, outputData + i*w + w));
            float const maxValue = static_cast<float>(*std::max_element(outputData + i*w, outputData+ i*w + w));// / partition;
            if (maxIndex > 0 && maxIndex < keySize && (!(i > 0 && maxIndex == lastIndex))) {
                scores.emplace_back(maxValue);
                strRes.append(keys_[maxIndex - 1]);
            }
            lastIndex = maxIndex;
        }
        return { strRes, scores };
    }

    TextLine get_text_line(const cv::Mat & src)
    {
        float scale = (float)dst_height_ / (float)src.rows;
        int dstWidth = int((float)src.cols * scale);

        cv::Mat srcResize;
        cv::resize(src, srcResize, cv::Size(dstWidth, dst_height_));
        //if you use PP-OCRv3 you should change PIXEL_RGB to PIXEL_RGB2BGR
        ncnn::Mat input = ncnn::Mat::from_pixels(srcResize.data, ncnn::Mat::PIXEL_RGB,srcResize.cols, srcResize.rows);
        float constexpr mean_vals[3] = { 127.5f, 127.5f, 127.5f };
        float constexpr norm_vals[3] = { 1.0f / 127.5f, 1.0f / 127.5f, 1.0f / 127.5f };
        input.substract_mean_normalize(mean_vals, norm_vals);

        ncnn::Extractor extractor = net_.create_extractor();
        extractor.input(input_name_.c_str(), input);

        ncnn::Mat out;
        extractor.extract(output_name_.c_str(), out);
        return score_to_text_line(static_cast<float*>(out.data), out.h, out.w);
    }

    std::vector<TextLine> get_text_lines(std::vector<cv::Mat> const &partImg) {
        size_t const size = partImg.size();
        std::vector<TextLine> textLines(size);
        for(int i = 0; i < size; ++i){
            TextLine textLine = get_text_line(partImg[i]);
            textLines[i] = textLine;
        }
        return textLines;
    }

    void predict(cv::Mat const &src, std::vector<TextBox>& textBoxes);

    int const dst_height_;
    bool has_gpu_ = false;
    std::vector<std::string> const keys_;
    int load_model_success_ = -1;
    int load_param_success_ = -1;
    int max_thread_ = 4;
    ncnn::Net net_;

    std::string input_name_;
    std::string output_name_;
};

paddle_ocr_text_rec::paddle_ocr_text_rec(const char *param, const char *bin, const char *key_files,
                                         int dst_height, bool use_gpu, int max_thread) :
    impl_{std::make_unique<impl>(param, bin, key_files, dst_height, use_gpu, max_thread)}
{

}

paddle_ocr_text_rec::~paddle_ocr_text_rec()
{

}

bool paddle_ocr_text_rec::get_load_model_success() const noexcept
{
    return impl_->load_model_success_;
}

bool paddle_ocr_text_rec::get_load_param_success() const noexcept
{
    return impl_->load_param_success_;
}

int paddle_ocr_text_rec::get_load_model_state() const noexcept
{
    if(get_load_model_success() == 0 && get_load_param_success() == 0){
        return 0;
    }
    if(get_load_model_success() == true && get_load_param_success() == false){
        return 1;
    }
    if(get_load_model_success() == false && get_load_param_success() == true){
        return 2;
    }
    if(get_load_model_success() == false && get_load_param_success() == false){
        return 3;
    }

    return -1;
}

void paddle_ocr_text_rec::predict(const cv::Mat &src, std::vector<TextBox> &text_boxes)
{
    impl_->predict(src, text_boxes);
}

void paddle_ocr_text_rec::impl::predict(const cv::Mat &src, std::vector<TextBox> &text_boxes)
{
    std::vector<cv::Mat> const part_images = getPartImages(src, text_boxes);
    std::vector<TextLine> text_lines = get_text_lines(part_images);

    for(int i = 0; i < text_lines.size(); i++){
        text_boxes[i].text = std::move(text_lines[i].text);
    }
}

}
