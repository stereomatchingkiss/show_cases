#include "crnn_process.h"
#include "db_post_process.h"

#include "paddle_ocr_det_opencv.hpp"
#include "paddle_ocr_rec_opencv.hpp"

#include <boost/algorithm/string.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>

#include <format>
#include <iostream>

cv::Mat Visualization(cv::Mat srcimg,
                      std::vector<flt::cvt::ocr::TextBox> const &boxes) {

    cv::Mat img_vis;
    srcimg.copyTo(img_vis);
    for (int n = 0; n < boxes.size(); n++) {
        cv::polylines(img_vis, boxes[n].boxPoint, 1, CV_RGB(0, 255, 0), 2, 8, 0);
    }

    cv::imshow("./vis.jpg", img_vis);
    cv::waitKey();
    std::cout << "The detection visualized image saved in ./vis.jpg" << std::endl;//*/
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
    show_text_det_result(det, "english-poetry-100696.png");
    show_text_det_result(det, "lite_demo.png");

    //auto input = cv::imread("english-poetry-100696.png");
    /*auto input = cv::imread("lite_demo.png");
    //cv::Mat blob = cv::dnn::blobFromImage( input, 1.f, cv::Size( 227, 227 ), cv::Scalar( 0, 0, 0 ), false, false );
    //std::cout<<blob.channels()<<", "<<blob.size<<","<<blob.type()<<std::endl;
    //std::cout<<CV_32F<<","<<CV_32FC3<<std::endl;

    //paddle_ocr_rec_opencv rec("ch_PP-OCRv3_rec_simple.onnx", "ppocr_keys_v1.txt");

    //cv::dnn::readNet("ch_PP-OCRv3_rec_simple.onnx");

    auto det_out = det.predict(input);
    Visualization(input, det_out);

    //rec.predict(input, det_out);//*/
}
