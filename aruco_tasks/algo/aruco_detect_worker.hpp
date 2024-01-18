#pragma once

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <memory>

struct config_aruco_detect_worker;

class aruco_detect_worker : public flt::mm::frame_process_base_worker
{
    Q_OBJECT
public:
    explicit aruco_detect_worker(config_aruco_detect_worker const &config, QObject *parent = nullptr);
    ~aruco_detect_worker();

private:
    void process_results(std::any frame) override;

    struct impl;
    std::unique_ptr<impl> impl_;
};
