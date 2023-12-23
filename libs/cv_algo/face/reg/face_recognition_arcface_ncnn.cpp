#include "face_recognition_arcface_ncnn.hpp"

#include "../../ncnn/ncnn_model_loader.hpp"

#include <net.h>

namespace flt::cvt::face{

struct face_recognition_arcface_ncnn::impl
{
    impl(std::string const &param, std::string const &bin, int target_size) :
        mloader_(param, bin, &net_),
        target_size_{target_size}
    {
        net_.opt.num_threads = 4;
        net_.opt.use_sgemm_convolution = 1;
        net_.opt.use_winograd_convolution = 1;
    }

    std::vector<float> predict(cv::Mat const &mat)
    {
        ncnn::Extractor ex = net_.create_extractor();
        //normalize and mean are done by the model, don't need to do it again
        ncnn::Mat input = ncnn::Mat::from_pixels_resize(mat.data, ncnn::Mat::PIXEL_BGR2RGB,
                                                        mat.cols, mat.rows,
                                                        target_size_, target_size_);
        ex.input(mloader_.get_input_name().c_str(), input);

        ncnn::Mat feat;
        ex.extract(mloader_.get_output_name().c_str(), feat);

        std::vector<float> results(feat.w);
        auto *ptr = static_cast<float*>(feat.channel(0).data);
        std::copy(ptr, ptr + feat.w, results.data());

        return results;
    }

    ncnn::Net net_;

    nc::ncnn_model_loader mloader_;
    int const target_size_;
};


face_recognition_arcface_ncnn::face_recognition_arcface_ncnn(std::string const &param, std::string const &bin, int target_size) :
    impl_{std::make_unique<impl>(param, bin, target_size)}
{    
}

face_recognition_arcface_ncnn::~face_recognition_arcface_ncnn()
{

}

std::vector<float> face_recognition_arcface_ncnn::predict(const cv::Mat &input)
{
    return impl_->predict(input);
}


}
