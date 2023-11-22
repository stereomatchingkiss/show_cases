#pragma once

#include <onnxruntime_cxx_api.h>

#include <string>
#include <vector>

namespace flt::cvt{

class onnx_get_names_utils
{
public:
    onnx_get_names_utils();

    std::vector<int64_t>& input_node_dims() noexcept;
    std::vector<Ort::AllocatedStringPtr> const& input_names_ptr() const noexcept;
    std::vector<const char*> const& input_node_names() const noexcept;

    std::vector<int64_t>& output_node_dims() noexcept;
    std::vector<Ort::AllocatedStringPtr> const& output_names_ptr() const noexcept;
    std::vector<const char*> const& output_node_names() const noexcept;

    void process(Ort::Session const &session);

    void print_model_details();

private:        
    void get_names(size_t node_index,
                   std::vector<int64_t> &node_dims,
                   std::vector<Ort::AllocatedStringPtr> &names_ptr,
                   std::vector<const char*> &node_names,
                   Ort::Session const &session,
                   bool input) const;

    void get_input_names(Ort::Session const &session);

    void get_output_names(Ort::Session const &session);

    std::vector<int64_t> input_node_dims_;
    std::vector<Ort::AllocatedStringPtr> input_names_ptr_;
    std::vector<const char*> input_node_names_;

    std::vector<int64_t> output_node_dims_;
    std::vector<Ort::AllocatedStringPtr> output_names_ptr_;
    std::vector<const char*> output_node_names_;
};

}
