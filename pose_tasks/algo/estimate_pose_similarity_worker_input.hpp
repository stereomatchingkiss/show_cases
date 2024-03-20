#pragma once

#include <QImage>
#include <QString>

struct estimate_pose_similarity_worker_input
{
    bool is_target_ = false;
    QImage qimg_;
};
