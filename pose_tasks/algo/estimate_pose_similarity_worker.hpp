#pragma once

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <memory>

struct config_psoe_estimation_worker;

class estimate_pose_similarity_worker : public flt::mm::frame_process_base_worker
{
    Q_OBJECT
public:
    explicit estimate_pose_similarity_worker(config_psoe_estimation_worker const &config, QObject *parent = nullptr);
    ~estimate_pose_similarity_worker();

private:
    void process_results(std::any input) override;

    struct impl;
    std::unique_ptr<impl> impl_;
};
