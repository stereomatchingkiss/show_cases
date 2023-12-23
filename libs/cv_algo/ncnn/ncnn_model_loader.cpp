#include "ncnn_model_loader.hpp"

#include <net.h>

namespace flt::cvt::nc{

ncnn_model_loader::ncnn_model_loader(std::string const &param, std::string const &bin, ncnn::Net *net)
{
    load_param_success_ = net->load_param(param.c_str());
    load_model_success_ = net->load_model(bin.c_str());

    input_name_ = net->input_names()[0];
    output_name_ = net->output_names()[net->output_names().size() - 1];
}

int ncnn_model_loader::get_load_model_state() const noexcept
{
    if(get_load_model_success() && get_load_param_success()){
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

const std::string &ncnn_model_loader::get_input_name() const noexcept
{
    return input_name_;
}

const std::string &ncnn_model_loader::get_output_name() const noexcept
{
    return output_name_;
}

bool ncnn_model_loader::get_load_param_success() const noexcept
{
    return load_param_success_ == 0;
}

bool ncnn_model_loader::get_load_model_success() const noexcept
{
    return load_model_success_ == 0;
}

}
