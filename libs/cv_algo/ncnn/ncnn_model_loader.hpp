#pragma once

#include <string>

namespace ncnn{

class Net;

}

namespace flt::cvt::nc{

class ncnn_model_loader
{
public:
    explicit ncnn_model_loader(std::string const &param, std::string const &bin, ncnn::Net *net);

    bool get_load_model_success() const noexcept;

    bool get_load_param_success() const noexcept;

    int get_load_model_state() const noexcept;

    std::string const& get_input_name() const noexcept;
    std::string const& get_output_name() const noexcept;

private:
    std::string input_name_;
    std::string output_name_;

    int load_model_success_ = -1;
    int load_param_success_ = -1;
};

}
