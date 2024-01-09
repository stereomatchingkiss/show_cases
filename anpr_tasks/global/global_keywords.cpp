#include "global_keywords.hpp"

#include <utils/file_reader.hpp>

std::string global_keywords::anpr_task_assets() const
{
#ifdef WASM_BUILD
    return "";
#else
    return "assets";
#endif
}

std::string global_keywords::nanodet_320_bin_path() const
{
    return anpr_task_assets() + "/license_plate_320.onnx_opt.bin";
}

std::string global_keywords::nanodet_320_param_path() const
{
    return anpr_task_assets() + "/license_plate_320.onnx_opt.param";
}

std::string global_keywords::nanodet_416_bin_path() const
{
    return anpr_task_assets() + "/license_plate_416.onnx_opt.bin";
}

std::string global_keywords::nanodet_416_param_path() const
{
    return anpr_task_assets() + "/license_plate_416.onnx_opt.param";
}

std::string global_keywords::paddle_ocr_rec_model_path() const
{
    return anpr_task_assets() + "/ch_PP-OCRv4_rec_simple_320.onnx";
}

QString global_keywords::state_stacks_manager() const
{
    return "state_stacks_manager";
}

QString global_keywords::state_widget_object_detect_model_select() const
{
    return "state_widget_object_detect_model_select";
}

QString global_keywords::state_widget_source_selection() const
{
    return "state_widget_source_selection";
}

QString global_keywords::state_version() const
{
    return "state_version";
}
