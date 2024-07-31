#pragma once

#include "single_frame_with_multi_worker_base.hpp"

#include <any>
#include <memory>

class QVideoFrame;

namespace flt::mm{

struct frame_capture_opencv_params;

class frame_capture_opencv : public single_frame_with_multi_worker_base
{
public:
    frame_capture_opencv(frame_capture_opencv_params params, QObject *parent = nullptr);
    ~frame_capture_opencv();

    void add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key) override;
    void remove_listener(void *key)  override;
    void start() override;

private:
    void send_process_results(std::any frame);

    struct impl;
    std::unique_ptr<impl> impl_;
};

}
