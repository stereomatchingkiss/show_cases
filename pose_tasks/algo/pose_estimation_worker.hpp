#pragma once

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <memory>

struct config_psoe_estimation_worker;

class pose_estimation_worker : public flt::mm::frame_process_base_worker
{
    Q_OBJECT
public:
    explicit pose_estimation_worker(config_psoe_estimation_worker const &config, QObject *parent = nullptr);
    ~pose_estimation_worker();

private:
    void process_results(std::any frame) override;

    struct impl;
    std::unique_ptr<impl> impl_;
};
