#include "frame_capture_controller.hpp"

#include "frame_capture_base_worker.hpp"

namespace ocv {

frame_capture_controller::frame_capture_controller(frame_capture_base_worker *worker, QObject *parent) :
    QObject(parent),
    worker_(worker)
{

}

}
