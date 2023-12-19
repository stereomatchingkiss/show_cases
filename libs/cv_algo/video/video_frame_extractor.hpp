#pragma once

#include <opencv2/core.hpp>

namespace flt::cvt::video{

class video_frame_extractor
{
public:
    /**
     * @param num_seg number of frames input to the model, which are extracted from a video.
     */
    explicit video_frame_extractor(int sampling_rate = 25, int num_seg = 8, bool input_is_rgb = false);

    bool enough_frames() const noexcept;
    bool extract(cv::Mat const &bgr);

    cv::Mat const& get_output_blob();
    int get_target_size() const noexcept;
    int get_frames_per_batch() const noexcept;    

private:
    void center_crop(cv::Mat const &input, cv::Mat &crop_img, int target_size) const;
    std::vector<cv::Mat> const& get_blobs() const noexcept;
    bool is_center_idx() const noexcept;
    /**
     * Not a best practice to combine two functions in one method, but this solution
     * can save useless operations.
     */
    void normalize(cv::Mat const &input, cv::Mat &output) const;
    cv::Mat preprocess(cv::Mat const &input) const;
    void resize(cv::Mat const &input, cv::Mat &resize_img, int short_size = 256) const;
    cv::Mat resize(cv::Mat const &input) const;

    int center_idx_;
    int frame_count_;    
    int frames_per_segment_;    
    std::vector<cv::Mat> input_blobs_;
    cv::Mat output_blob_;
    int num_seg_;
    int sampling_rate_;

    cv::Scalar_<float> const mean_ = {0.406f, 0.456f, 0.485f};
    cv::Scalar_<float> const scale_ = {0.225f, 0.224f, 0.229f};
    bool input_is_rgb_;
};

}
