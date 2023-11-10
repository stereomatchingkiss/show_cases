#pragma once

#include <QObject>
#include <QPixmap>

#include <any>

class QThread;

namespace flt::mm{

class frame_process_base_worker;

class frame_process_controller : public QObject
{
    Q_OBJECT
public:
    explicit frame_process_controller(frame_process_base_worker *worker, QObject *parent = nullptr);
    ~frame_process_controller();

    void predict(std::any results);
    void stop(bool val);

signals:
    void message_error(QString);    
    void send_process_results(std::any results);
    void start();

private:
    std::unique_ptr<QThread> thread_;
    frame_process_base_worker *worker_;
};

}
