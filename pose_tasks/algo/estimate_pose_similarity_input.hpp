#pragma once

#include <QImage>

struct estimate_pose_similarity_input
{
    bool is_target_ = false;
    QImage qimg_;
};
