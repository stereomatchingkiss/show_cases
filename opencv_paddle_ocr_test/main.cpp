#include <cv_algo/ocr/paddle_ocr/paddle_ocr_det_opencv.hpp>
#include <cv_algo/ocr/paddle_ocr/paddle_ocr_rec_onnx.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

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

void show_text_det_result(paddle_ocr_det_opencv &det, std::string const &im_name)
{
    auto input = cv::imread(im_name);
    auto det_out = det.predict(input);
    Visualization(input, det_out);
}

int main(int argc, char *argv[])
{    
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

    for(auto const &val : det_out){
        std::cout<<val.text<<std::endl;
    }
}

