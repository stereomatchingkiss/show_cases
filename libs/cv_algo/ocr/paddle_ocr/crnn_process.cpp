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

#include "crnn_process.hpp" //NOLINT

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <opencv2/imgproc.hpp>

namespace flt::cvt::ocr{

cv::Mat crnn_resize_img(cv::Mat const &img, float wh_ratio, int rec_image_height, int rec_max_width)
{
    int const imgW = int(rec_image_height * wh_ratio);
    float const ratio = float(img.cols) / float(img.rows);
    int const resize_w = (ceilf(rec_image_height * ratio) > imgW) ? imgW : static_cast<int>(ceilf(rec_image_height * ratio));

    cv::Mat resize_img;
    if(resize_w > rec_max_width){
        cv::resize(img, resize_img, cv::Size(rec_max_width, rec_image_height), 0.f, 0.f, cv::INTER_LINEAR);
    }else{
        cv::resize(img, resize_img, cv::Size(resize_w, rec_image_height), 0.f, 0.f, cv::INTER_LINEAR);
        cv::copyMakeBorder(resize_img, resize_img, 0, 0, 0,
                           int(imgW - resize_img.cols), cv::BORDER_CONSTANT,
                           {127, 127, 127});
    }
    return resize_img;
}

cv::Mat get_rotate_crop_image(cv::Mat const &srcimage, std::vector<cv::Point> const &box)
{
    auto const lf_it = std::ranges::minmax_element(box, [](auto const &a, auto const &b){ return a.x < b.x; });
    auto const tb_it = std::ranges::minmax_element(box, [](auto const &a, auto const &b){ return a.y < b.y; });
    int const left = lf_it.min->x;
    int const right = lf_it.max->x;
    int const top = tb_it.min->y;
    int const bottom = tb_it.max->y;

    cv::Mat const img_crop = srcimage(cv::Rect(left, top, right - left, bottom - top));
    cv::Point2f pointsf[4];
    for (int i = 0; i < box.size(); i++) {
        pointsf[i].x = static_cast<float>(box[i].x - left);
        pointsf[i].y = static_cast<float>(box[i].y - top);
    }

    int const img_crop_width =
        static_cast<int>(std::sqrt(std::pow(pointsf[0].x - pointsf[1].x, 2) +
                                   std::pow(pointsf[0].y - pointsf[1].y, 2)));
    int const img_crop_height =
        static_cast<int>(std::sqrt(std::pow(pointsf[0].x - pointsf[3].x, 2) +
                                   std::pow(pointsf[0].y - pointsf[3].y, 2)));

    cv::Point2f pts_std[4];
    pts_std[0] = cv::Point2f(0., 0.);
    pts_std[1] = cv::Point2f(static_cast<float>(img_crop_width), 0.f);
    pts_std[2] = cv::Point2f(static_cast<float>(img_crop_width), static_cast<float>(img_crop_height));
    pts_std[3] = cv::Point2f(0.f, static_cast<float>(img_crop_height));

    cv::Mat const M = cv::getPerspectiveTransform(pointsf, pts_std);
    cv::Mat dst_img;
    cv::warpPerspective(img_crop, dst_img, M,
                        cv::Size(img_crop_width, img_crop_height),
                        cv::BORDER_REPLICATE);

    float constexpr ratio = 1.5;
    if (static_cast<float>(dst_img.rows) >=
        static_cast<float>(dst_img.cols) * ratio) {
        cv::Mat srcCopy;
        cv::transpose(dst_img, srcCopy);
        cv::flip(srcCopy, srcCopy, 0);
        return srcCopy;
    } else {
        return dst_img;
    }
}

}
