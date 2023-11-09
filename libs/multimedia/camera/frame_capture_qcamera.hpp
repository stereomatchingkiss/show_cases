#pragma once

#include "single_frame_with_multi_worker_base.hpp"

#include <memory>

class QVideoFrame;

namespace flt::mm{

struct frame_capture_qcamera_params;

class frame_capture_qcamera : public single_frame_with_multi_worker_base
{
public:
    explicit frame_capture_qcamera(frame_capture_qcamera_params const &params, QObject *parent = nullptr);
    ~frame_capture_qcamera();

    void add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key) override;
    void remove_listener(void *key)  override;
    void start() override;

private:
    void video_frame_changed(QVideoFrame const &frame);

    struct impl;
    std::unique_ptr<impl> impl_;
};

}
