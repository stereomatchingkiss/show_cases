// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2021 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

//modified 1-14-2023 Q-engineering

#pragma once

#include <cv_algo/obj_detect/common_obj_det_type.hpp>

#include <opencv2/core/core.hpp>
#include <net.h>

namespace ocv::det{

class yolo_v8
{
public:
    explicit yolo_v8(const char* param,
                    const char* bin,
                    int num_class,
                    bool use_gpu = false,
                    int input_size = 320,
                    int max_thread = 4);

    std::vector<box_info> predict(cv::Mat const &rgb,
                                  float score_threshold = 0.4f,
                                  float nms_threshold = 0.5f,
                                  int rotation_angle = 0,
                                  bool hflip = false);
private:

    ncnn::Net net_;
    int target_size_;
    float const mean_vals_[3];
    float const norm_vals_[3];
    int num_class_;
};

}
