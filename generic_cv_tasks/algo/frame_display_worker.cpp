#include "frame_display_worker.hpp"

#include <QPixmap>

frame_display_worker::frame_display_worker(QObject *parent)
    : flt::mm::frame_process_base_worker{15, parent}
{

}

frame_display_worker::~frame_display_worker()
{

}

void frame_display_worker::process_results(std::any frame)
{
    auto qimg = std::any_cast<QImage>(frame);
    emit send_process_results(QPixmap::fromImage(std::move(qimg)));
}
