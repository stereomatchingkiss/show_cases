#include "video_classify_pptsm_opencv.hpp"

#include <opencv2/imgproc.hpp>

namespace flt::cvt::video{

video_classify_pptsm_opencv::video_classify_pptsm_opencv(std::string const &weight_path, int sampling_rate, int num_seg) :
    frame_extractor_(sampling_rate, num_seg),
    net_(cv::dnn::readNet(weight_path))
{    
}

video_classify_pptsm_opencv::~video_classify_pptsm_opencv()
{

}

std::vector<std::tuple<float, size_t>> video_classify_pptsm_opencv::predict(cv::Mat const &input, int top_k)
{
    if(frame_extractor_.extract(input) && frame_extractor_.enough_frames()){
        net_.setInput(frame_extractor_.get_output_blob());
        auto output = net_.forward();
        softmax(output.ptr<float>(0), output.ptr<float>(0) + 400);

        std::vector<std::tuple<float, size_t>> results;
        if(top_k == 1){
            auto it = std::max_element(output.ptr<float>(0), output.ptr<float>(0) + output.total());
            results.emplace_back(*it, std::distance(output.ptr<float>(0), it));
        }else{
            results.resize(output.total());
            auto const *out_ptr = output.ptr<float>(0);
            for(size_t i = 0; i != results.size(); ++i){
                results[i] = std::make_tuple(out_ptr[i], i);
            }

            std::ranges::sort(results, [](auto const &lhs, auto const &rhs)
                              {
                return std::get<0>(lhs) > std::get<0>(rhs);
            });

            results.resize(top_k);
        }

        return results;
    }

    return {};
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
