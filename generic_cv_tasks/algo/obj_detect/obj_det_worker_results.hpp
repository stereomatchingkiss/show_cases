#pragma once

#include <QImage>

struct obj_det_worker_results
{
    bool alarm_on_ = false;
    QImage mat_;
};
