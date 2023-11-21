#include "paddle_ocr_rec_onnx.hpp"

#include <cv_algo/ocr/paddle_ocr/paddle_ocr_det_opencv.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>

#include <format>
#include <iostream>

using namespace flt::cvt::ocr;

void run_ort_trt();

cv::Mat Visualization(cv::Mat srcimg,
                      std::vector<TextBox> const &boxes) {

    cv::Mat img_vis;
    srcimg.copyTo(img_vis);
    for (int n = 0; n < boxes.size(); n++) {
        cv::polylines(img_vis, boxes[n].boxPoint, 1, CV_RGB(0, 255, 0), 2, 8, 0);
    }

    cv::imshow("./vis.jpg", img_vis);
    cv::waitKey();
    std::cout << "The detection visualized image saved in ./vis.jpg" << std::endl;
    cv::destroyAllWindows();

    return img_vis;
}

void test_superres()
{
    cv::dnn::readNet("super_resolution.onnx");
}

void show_text_det_result(paddle_ocr_det_opencv &det, std::string const &im_name)
{
    auto input = cv::imread(im_name);
    auto det_out = det.predict(input);
    Visualization(input, det_out);
}

int main(int argc, char *argv[])
{
    //test_superres();
    paddle_ocr_det_opencv det("ch_PP-OCRv4_det_simple.onnx");
    //show_text_det_result(det, "english-poetry-100696.png");
    //show_text_det_result(det, "lite_demo.png");

    auto input = cv::imread("english-poetry-100696.png");
    auto det_out = det.predict(input);
    det_out.pop_back();
    det_out.pop_back();
    //Visualization(input, det_out);
    paddle_ocr_rec_onnx rec("ch_PP-OCRv4_rec_infer.onnx", "ppocr_keys_v1.txt");
    rec.predict(input, det_out);

    //run_ort_trt();

    //cv::dnn::readNet("ch_PP-OCRv3_rec_simple.onnx");

    //rec.predict(input, det_out);//*/
}//*/

#include <onnxruntime_cxx_api.h>

void run_ort_trt() {
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
    //const auto& api = Ort::GetApi();
    //OrtTensorRTProviderOptionsV2* tensorrt_options;

    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(1);

    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

#ifdef _WIN32
    const wchar_t* model_path = L"squeezenet.onnx";
#else
    const char* model_path = "squeezenet.onnx";
#endif

    //*****************************************************************************************
    // It's not suggested to directly new OrtTensorRTProviderOptionsV2 to get provider options
    //*****************************************************************************************
    //
    // auto tensorrt_options = get_default_trt_provider_options();
    // session_options.AppendExecutionProvider_TensorRT_V2(*tensorrt_options.get());

    //**************************************************************************************************************************
    // It's suggested to use CreateTensorRTProviderOptions() to get provider options
    // since ORT takes care of valid options for you
    //**************************************************************************************************************************
    //Ort::ThrowOnError(api.CreateTensorRTProviderOptions(&tensorrt_options));
    //std::unique_ptr<OrtTensorRTProviderOptionsV2, decltype(api.ReleaseTensorRTProviderOptions)> rel_trt_options(
    //    tensorrt_options, api.ReleaseTensorRTProviderOptions);
    //Ort::ThrowOnError(api.SessionOptionsAppendExecutionProvider_TensorRT_V2(static_cast<OrtSessionOptions*>(session_options),
    //                                                                        rel_trt_options.get()));

    std::cout << "Running ORT TRT EP with default provider options" << std::endl;

    Ort::Session session(env, model_path, session_options);

    //*************************************************************************
    // print model input layer (node names, types, shape etc.)
    Ort::AllocatorWithDefaultOptions allocator;

    // print number of model input nodes
    const size_t num_input_nodes = session.GetInputCount();
    std::vector<Ort::AllocatedStringPtr> input_names_ptr;
    std::vector<const char*> input_node_names;
    input_names_ptr.reserve(num_input_nodes);
    input_node_names.reserve(num_input_nodes);
    std::vector<int64_t> input_node_dims;  // simplify... this model has only 1 input node {1, 3, 224, 224}.
        // Otherwise need vector<vector<>>

    std::cout << "Number of inputs = " << num_input_nodes << std::endl;

    for(int i = 0; i != session.GetOutputCount(); ++i){
        std::cout<<"output "<<i<<" : "<<session.GetOutputNameAllocated(0, allocator)<<std::endl;
    }
    // iterate over all input nodes
    for (size_t i = 0; i < num_input_nodes; i++) {
        // print input node names
        auto input_name = session.GetInputNameAllocated(i, allocator);
        std::cout << "Input " << i << " : name =" << input_name.get() << std::endl;
        input_node_names.push_back(input_name.get());
        input_names_ptr.push_back(std::move(input_name));

        // print input node types
        auto type_info = session.GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();

        ONNXTensorElementDataType type = tensor_info.GetElementType();
        std::cout << "Input " << i << " : type = " << type << std::endl;

        // print input shapes/dims
        input_node_dims = tensor_info.GetShape();
        std::cout << "Input " << i << " : num_dims = " << input_node_dims.size() << '\n';
        for (size_t j = 0; j < input_node_dims.size(); j++) {
            std::cout << "Input " << i << " : dim[" << j << "] =" << input_node_dims[j] << '\n';
        }
        std::cout << std::flush;
    }//*/

    constexpr size_t input_tensor_size = 224 * 224 * 3;  // simplify ... using known dim values to calculate size
        // use OrtGetTensorShapeElementCount() to get official size!

    std::vector<float> input_tensor_values(input_tensor_size);
    std::vector<const char*> output_node_names = {"softmaxout_1"};

    // initialize input data with values in [0.0, 1.0]
    for (unsigned int i = 0; i < input_tensor_size; i++) input_tensor_values[i] = (float)i / (input_tensor_size + 1);

    // create input tensor object from data values
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    auto input_tensor = Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_size,
                                                        input_node_dims.data(), 4);
    assert(input_tensor.IsTensor());

    // score model & input tensor, get back output tensor
    auto output_tensors =
        session.Run(Ort::RunOptions{nullptr}, input_node_names.data(), &input_tensor, 1, output_node_names.data(), 1);
    assert(output_tensors.size() == 1 && output_tensors.front().IsTensor());

    // Get pointer to output tensor float values
    float* floatarr = output_tensors.front().GetTensorMutableData<float>();
    assert(abs(floatarr[0] - 0.000045) < 1e-6);

    // score the model, and print scores for first 5 classes
    for (int i = 0; i < 5; i++) {
        std::cout << "Score for class [" << i << "] =  " << floatarr[i] << '\n';
    }
    std::cout << std::flush;

    // Results should be as below...
    // Score for class[0] = 0.000045
    // Score for class[1] = 0.003846
    // Score for class[2] = 0.000125
    // Score for class[3] = 0.001180
    // Score for class[4] = 0.001317

    std::cout << "Done!" << std::endl;//*/
}

