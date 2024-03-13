#include "pose_estimation_utils.hpp"

#include <opencv2/imgproc.hpp>

namespace flt::cvt::pose{

void draw(cv::Mat& bgr, std::vector<keypoint> const &points, float min_confidence)
{
    size_t constexpr skele_index[][2] = { {0,1},{0,2},{1,3},{2,4},{0,5},{0,6},{5,6},{5,7},{7,9},{6,8},{8,10},{11,12},
                                         {5,11},{11,13},{13,15},{6,12},{12,14},{14,16} };
    int constexpr color_index[][3] = { {255, 0, 0},
        {0, 0, 255},
        {255, 0, 0},
        {0, 0, 255},
        {255, 0, 0},
        {0, 0, 255},
        {0, 255, 0},
        {255, 0, 0},
        {255, 0, 0},
        {0, 0, 255},
        {0, 0, 255},
        {0, 255, 0},
        {255, 0, 0},
        {255, 0, 0},
        {255, 0, 0},
        {0, 0, 255},
        {0, 0, 255},
        {0, 0, 255}, };

    for (int i = 0; i < 18; ++i){
        if(points[skele_index[i][0]].score_ > min_confidence && points[skele_index[i][1]].score_ > min_confidence){
            cv::line(bgr, cv::Point(points[skele_index[i][0]].x_,points[skele_index[i][0]].y_),
                     cv::Point(points[skele_index[i][1]].x_,points[skele_index[i][1]].y_),
                     cv::Scalar(color_index[i][0], color_index[i][1], color_index[i][2]), 2);
        }
    }

    int constexpr num_joints = 17;
    for(int i = 0; i < num_joints; ++i){
        if (points[i].score_ > 0.3f){
            cv::circle(bgr, cv::Point(points[i].x_,points[i].y_), 3, cv::Scalar(100, 255, 150), -1);
        }
    }
}

}
