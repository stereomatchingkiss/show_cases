#ifndef NANODET_WORKER_HPP
#define NANODET_WORKER_HPP

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <QRectF>

#include <memory>

class nanodet_worker : public ocv::frame_process_base_worker
{
    Q_OBJECT
public:
    explicit nanodet_worker(QRectF const &rband,
                            float score_threshold = 0.4f,
                            float nms_threshold = 0.3f,
                            int input_size = 416,
                            QObject *parent = nullptr);
    ~nanodet_worker();

    void process_results(std::any frame) override;

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

#endif // NANODET_WORKER_HPP
