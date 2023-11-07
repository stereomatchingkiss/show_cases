#pragma once

#include "single_frame_with_multi_worker_base.hpp"

#include <QObject>

#include <any>
#include <memory>

namespace flt::mm{

class frame_capture_controller;
class frame_capture_opencv_worker;
class frame_process_controller;
class frame_process_base_worker;

struct frame_capture_params;

class single_frame_with_multi_worker : public single_frame_with_multi_worker_base
{
    Q_OBJECT
public:
    explicit single_frame_with_multi_worker(frame_capture_params const &params,
                                            QObject *parent = nullptr);
    ~single_frame_with_multi_worker();

    void add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key) override;
    void remove_listener(void *key) override;
    void start() override;

signals:
    void message_error(QString msg);    

private:
    void stop();

    std::unique_ptr<frame_capture_controller> frame_capture_controller_;
    std::vector<std::pair<std::shared_ptr<frame_process_controller>, void*>> frame_process_vector_;
};

}
