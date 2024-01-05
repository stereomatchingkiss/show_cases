#include "paddle_ocr_rec_opencv.hpp"

#include "crnn_process.hpp"

#include "../../../utils/file_reader.hpp"

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>

namespace flt::cvt::ocr{

struct paddle_ocr_rec_opencv::impl
{
    impl(std::string const &model_weights,
         std::string const &key_files,
         int dst_height = 48,
         int width = 320) :
        dst_height_{dst_height},
        dst_width_{width},
        keys_{read_file_per_lines(key_files)}
    {
        net_ = cv::dnn::readNet(model_weights);

        keys_.insert(keys_.begin(), "#"); // blank char for ctc
        keys_.push_back(" ");
    }

    void predict(cv::Mat const &bgr, std::vector<TextBox> &boxes)
    {
        for(size_t i = 0; i != boxes.size(); ++i){

            auto const resize_img = preprocess(bgr, boxes[i].boxPoint);
            cv::dnn::blobFromImage(resize_img, input_blob_, 1 / 255.0, {}, mean_);
            input_blob_ /= 0.5;
            net_.setInput(input_blob_);
            cv::Mat const output = net_.forward();

            process_rec_results(boxes[i], output.ptr<float>(), output.size().width, keys_.size());
        }
    }

    cv::Mat preprocess(cv::Mat const &mat, std::vector<cv::Point> const &box_points) const
    {
        auto crop_img = get_rotate_crop_image(mat, box_points);
        cv::Mat resize_img;
        cv::resize(crop_img, resize_img, cv::Size(dst_width_, dst_height_), 0.f, 0.f, cv::INTER_LINEAR);

        return resize_img;
    }

    void process_rec_results(TextBox &tbox, float const *floatarr, size_t candidate, size_t elem_size = 6625) const
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

    int dst_height_;
    int dst_width_;
    cv::Mat input_blob_;
    std::vector<float> input_data_;
    std::vector<std::string> keys_;
    cv::Scalar const mean_{0.5, 0.5, 0.5};
    cv::dnn::Net net_;
};

paddle_ocr_rec_opencv::paddle_ocr_rec_opencv(std::string const &model_weights,
                                             std::string const &key_files,
                                             int dst_height,
                                             int width) :
    impl_{std::make_unique<impl>(model_weights, key_files, dst_height, width)}
{

}

paddle_ocr_rec_opencv::~paddle_ocr_rec_opencv()
{

}

void paddle_ocr_rec_opencv::predict(const cv::Mat &bgr, std::vector<TextBox> &text_boxes)
{
    impl_->predict(bgr, text_boxes);
}

}
