#pragma once

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <memory>

struct config_estimate_many_pose_similarity_worker;

class estimate_many_pose_similarity_worker : public flt::mm::frame_process_base_worker
{
    Q_OBJECT
public:
    explicit estimate_many_pose_similarity_worker(config_estimate_many_pose_similarity_worker const &config,
                                                  QObject *parent = nullptr);
    ~estimate_many_pose_similarity_worker();

signals:
    void send_msg(QString info);
    void send_request_image(QImage img);
    void send_similar_pose(std::any info);

private:
    void process_results(std::any input) override;

    struct impl;
    std::unique_ptr<impl> impl_;
};
