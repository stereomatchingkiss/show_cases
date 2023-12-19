#include "video_classify_pptsm_opencv.hpp"

#include <opencv2/imgproc.hpp>

namespace flt::cvt::video{

video_classify_pptsm_opencv::video_classify_pptsm_opencv(std::string const &weight_path, int sampling_rate, int num_seg, bool swap_rgb) :
    frame_extractor_{sampling_rate, num_seg},
    net_{cv::dnn::readNet(weight_path)},
    input_is_rgb_{swap_rgb}
{    
}

video_classify_pptsm_opencv::~video_classify_pptsm_opencv()
{

}

std::vector<std::tuple<float, size_t>> video_classify_pptsm_opencv::predict(cv::Mat const &bgr, int top_k)
{
    if(frame_extractor_.extract(bgr) && frame_extractor_.enough_frames()){
        net_.setInput(frame_extractor_.get_output_blob());
        auto output = net_.forward();
        softmax(output.ptr<float>(0), output.ptr<float>(0) + 400);
        results_.clear();

        if(top_k == 1){
            auto it = std::max_element(output.ptr<float>(0), output.ptr<float>(0) + output.total());
            results_.emplace_back(*it, std::distance(output.ptr<float>(0), it));
        }else{
            results_.resize(output.total());
            auto const *out_ptr = output.ptr<float>(0);
            for(size_t i = 0; i != results_.size(); ++i){
                results_[i] = std::make_tuple(out_ptr[i], i);
            }

            std::ranges::sort(results_, [](auto const &lhs, auto const &rhs)
                              {
                return std::get<0>(lhs) > std::get<0>(rhs);
            });

            results_.resize(top_k);
        }

        return results_;
    }

    return results_;
}

void video_classify_pptsm_opencv::softmax(float *begin, float *end) const noexcept
{
    float const max_value = *std::max_element(begin, end);
    float denominator = 0.0f;
    for(auto it = begin; it != end; ++it){
        *it = std::exp((*it) - max_value);
        denominator += (*it);
    }
    for(auto it = begin; it != end; ++it){
        *it /= denominator;
    }
}

}
