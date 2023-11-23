#include "onnx_get_names_utils.hpp"

#ifndef WASM_BUILD

#include <iostream>

namespace flt::cvt{

onnx_get_names_utils::onnx_get_names_utils()
{

}

std::vector<int64_t>& onnx_get_names_utils::input_node_dims() noexcept
{
    return input_node_dims_;
}

const std::vector<Ort::AllocatedStringPtr> &onnx_get_names_utils::input_names_ptr() const noexcept
{
    return input_names_ptr_;
}

const std::vector<const char *> &onnx_get_names_utils::input_node_names() const noexcept
{
    return input_node_names_;
}

std::vector<int64_t>& onnx_get_names_utils::output_node_dims() noexcept
{
    return output_node_dims_;
}

const std::vector<Ort::AllocatedStringPtr> &onnx_get_names_utils::output_names_ptr() const noexcept
{
    return output_names_ptr_;
}

const std::vector<const char *> &onnx_get_names_utils::output_node_names() const noexcept
{
    return output_node_names_;
}

void onnx_get_names_utils::process(const Ort::Session &session)
{
    get_input_names(session);
    get_output_names(session);
}

void onnx_get_names_utils::print_model_details()
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

void onnx_get_names_utils::get_names(size_t node_index, std::vector<int64_t> &node_dims, std::vector<Ort::AllocatedStringPtr> &names_ptr, std::vector<const char *> &node_names, const Ort::Session &session, bool input) const
{
    size_t const node_counts = input ? session.GetInputCount() : session.GetOutputCount();
    names_ptr.reserve(node_counts);
    node_names.reserve(node_counts);

    Ort::AllocatorWithDefaultOptions allocator;
    auto node_name = input ? session.GetInputNameAllocated(node_index, allocator) :
                         session.GetOutputNameAllocated(node_index, allocator);
    node_names.push_back(node_name.get());
    names_ptr.push_back(std::move(node_name));

    auto const type_info = session.GetInputTypeInfo(node_index);
    auto const tensor_info = type_info.GetTensorTypeAndShapeInfo();

    node_dims = tensor_info.GetShape();
}

void onnx_get_names_utils::get_input_names(const Ort::Session &session)
{
    get_names(0, input_node_dims_, input_names_ptr_, input_node_names_, session, true);
}

void onnx_get_names_utils::get_output_names(const Ort::Session &session)
{
    get_names(0, output_node_dims_, output_names_ptr_, output_node_names_, session, false);
}

}

#endif
