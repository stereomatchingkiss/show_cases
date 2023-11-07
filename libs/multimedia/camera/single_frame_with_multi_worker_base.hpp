#pragma once

#include <QObject>

namespace flt::mm{

class frame_process_controller;

class single_frame_with_multi_worker_base : public QObject
{
public:
    explicit single_frame_with_multi_worker_base(QObject *parent = nullptr);
    ~single_frame_with_multi_worker_base();

    virtual void add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key) = 0;
    virtual void remove_listener(void *key)  = 0;
    virtual void start()  = 0;
};

}
