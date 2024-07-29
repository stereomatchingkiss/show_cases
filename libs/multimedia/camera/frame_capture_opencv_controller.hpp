#pragma once

#include <any>

#include <QObject>

class QThread;

namespace flt::mm{

class frame_capture_opencv_worker;

class frame_capture_opencv_controller : public QObject
{
    Q_OBJECT
public:
    frame_capture_opencv_controller(frame_capture_opencv_worker *worker, QObject *parent = nullptr);
    ~frame_capture_opencv_controller();

    void stop(bool val);

signals:
    void message_error(QString);

    void send_process_results(std::any frame);
    void start();

private:
    std::unique_ptr<QThread> thread_;
    frame_capture_opencv_worker *worker_;
};

}
