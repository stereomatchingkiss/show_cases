#pragma once

#include "../../config/config_nanodet_worker.hpp"

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <QRectF>

#include <memory>

struct config_nanodet_worker;

class nanodet_worker : public flt::mm::frame_process_base_worker
{
    Q_OBJECT
public:
    explicit nanodet_worker(config_nanodet_worker config, QObject *parent = nullptr);
    ~nanodet_worker();

    void process_results(std::any frame) override;

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};
