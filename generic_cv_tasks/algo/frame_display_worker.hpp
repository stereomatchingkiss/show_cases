#ifndef FRAME_DISPLAY_WORKER_HPP
#define FRAME_DISPLAY_WORKER_HPP

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <QObject>

class frame_display_worker : public flt::mm::frame_process_base_worker
{
    Q_OBJECT
public:
    explicit frame_display_worker(QObject *parent = nullptr);
    ~frame_display_worker();

    void process_results(std::any frame) override;
};

#endif // FRAME_DISPLAY_WORKER_HPP
