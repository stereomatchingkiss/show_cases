// Copyright (c) 2020 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <cv_algo/ocr/paddle_ocr/common.hpp>

#include <opencv2/core.hpp>

namespace flt::cvt::ocr{

std::vector<std::vector<float>> Mat2Vector(cv::Mat const &mat);

void GetContourArea(std::vector<std::vector<float>> const &box,
                    float unclip_ratio,
                    float &distance);

cv::RotatedRect Unclip(std::vector<std::vector<float>> const &box, float unclip_ratio);

std::vector<std::vector<float>> Mat2Vector(cv::Mat const &mat);

inline bool XsortFp32(std::vector<float> const &a, std::vector<float> const &b) {
    if (a[0] != b[0])
        return a[0] < b[0];
    return false;
}

inline bool XsortInt(std::vector<int> const &a, std::vector<int> const &b) {
    if (a[0] != b[0])
        return a[0] < b[0];
    return false;
}

std::vector<std::vector<float>> GetMiniBoxes(cv::RotatedRect const &box, float &ssid);

float BoxScoreFast(std::vector<std::vector<float>> const &box_array, cv::Mat const &pred);

std::vector<TextBox> BoxesFromBitmap(cv::Mat const &pred,
                                     cv::Mat const &bitmap,
                                     float box_thresh,
                                     float unclip_ratio);

std::vector<TextBox> FilterTagDetRes(std::vector<TextBox> &boxes,
                                     float ratio_h,
                                     float ratio_w,
                                     int rows,
                                     int cols);

}
