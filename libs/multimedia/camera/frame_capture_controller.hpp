#pragma once

#include <QObject>
#include <QString>

#include <any>
#include <functional>
#include <memory>

class QThread;

namespace ocv::mm{

class frame_capture_base_worker;

class frame_capture_controller : public QObject
{
    Q_OBJECT
public:
    explicit frame_capture_controller(frame_capture_base_worker *worker, QObject *parent = nullptr);
    ~frame_capture_controller();

    void add_listener(std::function<void(std::any)> func, void *key);
    void clear();
    void remove_listener(void *key);
    void stop();

signals:    
    void message_error(QString);
    void start();    

private:
    std::unique_ptr<QThread> thread_;
    frame_capture_base_worker *worker_;
};

}

Q_DECLARE_METATYPE(std::function<void(std::any)>)
