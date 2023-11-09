#pragma once

#include <QImage>
#include <QPixmap>
#include <QString>

#include <opencv2/core.hpp>

struct obj_det_worker_results
{
    bool alarm_on_ = false;
    QImage mat_;
};
