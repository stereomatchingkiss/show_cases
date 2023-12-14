#include <cv_algo/video/video_classify_pptsm_opencv.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

using namespace flt::cvt::video;
using namespace std;

int main()
{
    std::unique_ptr<video_classify_base> model = std::make_unique<video_classify_pptsm_opencv>("ppTSMv2_8f_simple.onnx", 1);
    cv::VideoCapture cap("../../../3rdLibs/paddle_paddle/PaddleVideo/data/example.avi");
    while(1){
        cv::Mat frame;        
        if(cap>>frame; !frame.empty()){
            if(auto const results = model->predict(frame, 1); !results.empty()){
                for(size_t i = 0; i != results.size(); ++i){
                    cout<<std::get<0>(results[i])<<","<<std::get<1>(results[i])<<endl;
                }
            }
        }else{
            break;
        }
    }    

    return 0;
}
