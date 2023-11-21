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

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace flt::cvt::ocr{

cv::Mat CrnnResizeImg(cv::Mat const &img, float wh_ratio, int rec_image_height)
{
    int const imgW = int(rec_image_height * wh_ratio);
    float const ratio = float(img.cols) / float(img.rows);
    int const resize_w = (ceilf(rec_image_height * ratio) > imgW) ? imgW : static_cast<int>(ceilf(rec_image_height * ratio));

    cv::Mat resize_img;
    cv::resize(img, resize_img, cv::Size(resize_w, rec_image_height), 0.f, 0.f, cv::INTER_LINEAR);
    /*cv::copyMakeBorder(resize_img, resize_img, 0, 0, 0,
                       int(imgW - resize_img.cols), cv::BORDER_CONSTANT,
                       {127, 127, 127});//*/
    return resize_img;
}

std::vector<std::string> ReadDict(std::string const &path)
{

    std::vector<std::string> m_vec;
    if(std::ifstream in(path); in){
        std::string line;
        while(getline(in, line)){
            m_vec.emplace_back(std::move(line));
        }
    }else{
        std::cout << "no such file" << std::endl;
    }

    return m_vec;
}

cv::Mat GetRotateCropImage(cv::Mat const &srcimage, std::vector<cv::Point> const &box)
{
    int const x_collect[4] = {box[0].x, box[1].x, box[2].x, box[3].x};
    int const y_collect[4] = {box[0].y, box[1].y, box[2].y, box[3].y};
    int const left = int(*std::min_element(x_collect, x_collect + 4));
    int const right = int(*std::max_element(x_collect, x_collect + 4));
    int const top = int(*std::min_element(y_collect, y_collect + 4));
    int const bottom = int(*std::max_element(y_collect, y_collect + 4));

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
