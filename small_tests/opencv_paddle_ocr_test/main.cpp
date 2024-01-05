#include <cv_algo/ocr/paddle_ocr/paddle_ocr_det_opencv.hpp>
#include <cv_algo/ocr/paddle_ocr/paddle_ocr_rec_onnx.hpp>
#include <cv_algo/ocr/paddle_ocr/paddle_ocr_rec_opencv.hpp>

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
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

template<typename T>
void detect_text(std::string const &im_name, paddle_ocr_det_opencv &det, T &rec)
{
    auto input = cv::imread(im_name);
    auto det_out = det.predict(input);
    rec.predict(input, det_out);
    //Visualization(input, det_out);

    for(auto const &val : det_out){
        std::cout<<val.text<<std::endl;
    }
}

int main(int argc, char *argv[])
{    
    paddle_ocr_det_opencv det("ch_PP-OCRv4_det_simple.onnx");
    //paddle_ocr_rec_onnx rec("ch_PP-OCRv4_rec_infer.onnx", "ppocr_keys_v1.txt");

    paddle_ocr_rec_opencv rec("ch_PP-OCRv4_rec_simple.onnx", "ppocr_keys_v1.txt");

    //resize_w = 498
    //resize_w = 375
    //resize_w = 489
    //resize_w = 375
    //resize_w = 85
    //resize_w = 55
    detect_text("english-poetry-100696.png", det, rec);

    //detect_text("english.png", det, rec);

    //cv::dnn::readNet("ch_PP-OCRv4_rec_simple.onnx");

    /*for(size_t i = 0; i != 10; ++i){
        detect_text("english-poetry-100696.png", det, rec);
        detect_text("english.png", det, rec);
        detect_text("lite_demo.png", det, rec);
    }//*/
}

