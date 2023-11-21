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

#include "db_post_process.h" // NOLINT

#include <cv_algo/ocr/paddle_ocr/clipper.hpp>

#include <cmath>
#include <algorithm>
#include <format>
#include <utility>

#include <opencv2/imgproc.hpp>

namespace{

template <class T> inline T clamp(T x, T min, T max) {
    return std::clamp(x, min, max);
}

}

void GetContourArea(std::vector<std::vector<float>> const &box, float unclip_ratio,
                    float &distance) {
    int pts_num = 4;
    float area = 0.0f;
    float dist = 0.0f;
    for (int i = 0; i < pts_num; i++) {
        area += box[i][0] * box[(i + 1) % pts_num][1] -
                box[i][1] * box[(i + 1) % pts_num][0];
        dist += sqrtf((box[i][0] - box[(i + 1) % pts_num][0]) *
                          (box[i][0] - box[(i + 1) % pts_num][0]) +
                      (box[i][1] - box[(i + 1) % pts_num][1]) *
                          (box[i][1] - box[(i + 1) % pts_num][1]));
    }
    area = std::fabs(float(area / 2.0));

    distance = area * unclip_ratio / dist;
}

cv::RotatedRect Unclip(std::vector<std::vector<float>> const &box,
                       float unclip_ratio) {
    float distance = 1.0;

    GetContourArea(box, unclip_ratio, distance);

    flt::cvt::ocr::ClipperOffset offset;
    flt::cvt::ocr::Path p;
    p << flt::cvt::ocr::IntPoint(static_cast<int>(box[0][0]),
                                 static_cast<int>(box[0][1]))
      << flt::cvt::ocr::IntPoint(static_cast<int>(box[1][0]),
                                 static_cast<int>(box[1][1]))
      << flt::cvt::ocr::IntPoint(static_cast<int>(box[2][0]),
                                 static_cast<int>(box[2][1]))
      << flt::cvt::ocr::IntPoint(static_cast<int>(box[3][0]),
                                 static_cast<int>(box[3][1]));
    offset.AddPath(p, flt::cvt::ocr::jtRound, flt::cvt::ocr::etClosedPolygon);

    flt::cvt::ocr::Paths soln;
    offset.Execute(soln, distance);
    std::vector<cv::Point2f> points;

    for (int j = 0; j < soln.size(); j++) {
        for (int i = 0; i < soln[soln.size() - 1].size(); i++) {
            points.emplace_back(soln[j][i].X, soln[j][i].Y);
        }
    }

    return cv::minAreaRect(points);
}

std::vector<std::vector<float>> Mat2Vector(cv::Mat const &mat)
{
    std::vector<std::vector<float>> img_vec;
    for(int i = 0; i < mat.rows; ++i){
        auto ptr = mat.ptr<float>(i);
        std::vector<float> tmp;
        for (int j = 0; j < mat.cols; ++j){
            tmp.emplace_back(ptr[j]);
        }
        img_vec.emplace_back(std::move(tmp));
    }

    return img_vec;
}

std::vector<std::vector<float>> GetMiniBoxes(cv::RotatedRect const &box, float &ssid) {
    ssid = std::min(box.size.width, box.size.height);

    cv::Mat points;
    cv::boxPoints(box, points);

    auto array = Mat2Vector(points);
    std::sort(array.begin(), array.end(), XsortFp32);

    std::vector<float> idx1, idx2, idx3, idx4;
    if (array[3][1] <= array[2][1]) {
        idx2 = std::move(array[3]);
        idx3 = std::move(array[2]);
    } else {
        idx2 = std::move(array[2]);
        idx3 = std::move(array[3]);
    }
    if (array[1][1] <= array[0][1]) {
        idx1 = std::move(array[1]);
        idx4 = std::move(array[0]);
    } else {
        idx1 = std::move(array[0]);
        idx4 = std::move(array[1]);
    }

    array[0] = std::move(idx1);
    array[1] = std::move(idx2);
    array[2] = std::move(idx3);
    array[3] = std::move(idx4);

    return array;
}

float BoxScoreFast(std::vector<std::vector<float>> const &box_array, cv::Mat const &pred) {
    auto const &array = box_array;
    int width = pred.cols;
    int height = pred.rows;

    float const box_x[4] = {array[0][0], array[1][0], array[2][0], array[3][0]};
    float const box_y[4] = {array[0][1], array[1][1], array[2][1], array[3][1]};

    int const xmin = clamp(
        static_cast<int>(::floorf(*(std::min_element(box_x, box_x + 4)))), 0,
        width - 1);
    int const xmax =
        clamp(static_cast<int>(::ceilf(*(std::max_element(box_x, box_x + 4)))),
              0, width - 1);
    int const ymin = clamp(
        static_cast<int>(::floorf(*(std::min_element(box_y, box_y + 4)))), 0,
        height - 1);
    int const ymax =
        clamp(static_cast<int>(::ceilf(*(std::max_element(box_y, box_y + 4)))),
              0, height - 1);

    cv::Mat const mask = cv::Mat::zeros(ymax - ymin + 1, xmax - xmin + 1, CV_8UC1);

    cv::Point root_point[4];
    root_point[0] = cv::Point(static_cast<int>(array[0][0]) - xmin,
                              static_cast<int>(array[0][1]) - ymin);
    root_point[1] = cv::Point(static_cast<int>(array[1][0]) - xmin,
                              static_cast<int>(array[1][1]) - ymin);
    root_point[2] = cv::Point(static_cast<int>(array[2][0]) - xmin,
                              static_cast<int>(array[2][1]) - ymin);
    root_point[3] = cv::Point(static_cast<int>(array[3][0]) - xmin,
                              static_cast<int>(array[3][1]) - ymin);
    const cv::Point *ppt[1] = {root_point};
    int npt[] = {4};
    cv::fillPoly(mask, ppt, npt, 1, cv::Scalar(1));

    cv::Mat croppedImg;
    pred(cv::Rect(xmin, ymin, xmax - xmin + 1, ymax - ymin + 1))
        .copyTo(croppedImg);

    return static_cast<float>(cv::mean(croppedImg, mask)[0]);

}

std::vector<flt::cvt::ocr::TextBox>
BoxesFromBitmap(cv::Mat const &pred,
                cv::Mat const &bitmap,
                float box_thresh,
                float unclip_ratio)
{
    int constexpr min_size = 3;
    int constexpr max_candidates = 1000;

    int width = bitmap.cols;
    int height = bitmap.rows;

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(bitmap, contours, hierarchy, cv::RETR_LIST,
                     cv::CHAIN_APPROX_SIMPLE);

    size_t const num_contours = contours.size() >= max_candidates ? max_candidates : contours.size();

    std::vector<flt::cvt::ocr::TextBox> boxes;

    for(size_t i = 0; i < num_contours; i++) {
        if (contours[i].size() <= 2)
            continue;

        cv::RotatedRect const box = cv::minAreaRect(contours[i]);
        float ssid;
        auto array = GetMiniBoxes(box, ssid);

        auto box_for_unclip = array;
        // end get_mini_box

        if (ssid < min_size) {
            continue;
        }

        float const score = BoxScoreFast(array, pred);
        if (score < box_thresh)
            continue;

        // start for unclip
        cv::RotatedRect points = Unclip(box_for_unclip, unclip_ratio);
        if (points.size.height < 1.001 && points.size.width < 1.001)
            continue;
        // end for unclip

        cv::RotatedRect clipbox = points;
        auto cliparray = GetMiniBoxes(clipbox, ssid);

        if (ssid < min_size + 2)
            continue;

        int dest_width = pred.cols;
        int dest_height = pred.rows;
        flt::cvt::ocr::TextBox intcliparray;

        for(size_t num_pt = 0; num_pt < 4; ++num_pt) {
            intcliparray.boxPoint.emplace_back(static_cast<int>(clamp(
                                                   std::roundf(cliparray[num_pt][0] / float(width) * float(dest_width)),
                                                   float(0), float(dest_width))),
                                               static_cast<int>(clamp(
                                                   std::roundf(cliparray[num_pt][1] / float(height) * float(dest_height)),
                                                   float(0), float(dest_height))));
        }

        boxes.emplace_back(std::move(intcliparray));

    } // end for
    return boxes;
}

std::vector<flt::cvt::ocr::TextBox>
FilterTagDetRes(std::vector<flt::cvt::ocr::TextBox> &boxes,
                float ratio_h,
                float ratio_w,
                int rows,
                int cols)
{
    for (size_t n = 0; n < boxes.size(); ++n) {
        auto &bpoints = boxes[n].boxPoint;
        for(size_t m = 0; m < bpoints.size(); ++m) {
            bpoints[m].x /= ratio_w;
            bpoints[m].y /= ratio_h;

            bpoints[m].x = std::clamp(bpoints[m].x, 0, cols - 1);
            bpoints[m].y = std::clamp(bpoints[m].y, 0, rows - 1);
        }
    }

    std::vector<flt::cvt::ocr::TextBox> root_points;
    for(size_t n = 0; n < boxes.size(); ++n) {
        auto &bpoints = boxes[n].boxPoint;
        int const rect_width =
            static_cast<int>(std::sqrt(std::pow(bpoints[0].x - bpoints[1].x, 2) +
                                       std::pow(bpoints[0].y - bpoints[1].y, 2)));
        int const rect_height =
            static_cast<int>(std::sqrt(std::pow(bpoints[0].x - bpoints[3].x, 2) +
                                       std::pow(bpoints[0].y - bpoints[3].y, 2)));

        if (rect_width <= 4 || rect_height <= 4)
            continue;
        root_points.push_back(std::move(boxes[n]));
    }

    return root_points;
}
