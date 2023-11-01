#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

namespace flt::cvt::ocr {

struct TextBox {
    std::vector<cv::Point> boxPoint;
    float score;
    std::string text;
};
struct TextLine {
    std::string text;
    std::vector<float> charScores;
};

std::vector<cv::Point> getMinBoxes(const std::vector<cv::Point>& inVec, float& minSideLen, float& allEdgeSize);
float boxScoreFast(const cv::Mat& inMat, const std::vector<cv::Point>& inBox);
std::vector<cv::Point> unClip(const std::vector<cv::Point>& inBox, float perimeter, float unClipRatio);
cv::Mat getRotateCropImage(const cv::Mat& src, std::vector<cv::Point> box);
std::vector<cv::Mat> getPartImages(const cv::Mat& src, std::vector<TextBox>& textBoxes);
cv::Mat matRotateClockWise180(cv::Mat src);
cv::Mat makePadding(cv::Mat& src, const int padding);

}
