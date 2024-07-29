#pragma once

#include <QObject>

#include <any>
#include <memory>

namespace flt::mm{

struct frame_capture_opencv_params;

class frame_capture_opencv_worker : public QObject
{
    Q_OBJECT
public:
    explicit frame_capture_opencv_worker(frame_capture_opencv_params params, QObject *parent = nullptr);
    ~frame_capture_opencv_worker();

    void stop(bool val);
    void start();

signals:
    void message_error(QString);

    void send_process_results(std::any frame);

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}
