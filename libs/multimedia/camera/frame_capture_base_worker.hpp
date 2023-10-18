#ifndef FRAME_CAPTURE_BASE_WORKER_HPP
#define FRAME_CAPTURE_BASE_WORKER_HPP

#include <any>
#include <functional>
#include <memory>

#include <QObject>
#include <QString>

namespace ocv{

struct frame_capture_params;

class frame_capture_base_worker : public QObject
{
    Q_OBJECT
public:
    explicit frame_capture_base_worker(frame_capture_params params, QObject *parent = nullptr);
    ~frame_capture_base_worker();

    void add_listener(std::function<void(std::any)> func, void *key);
    void call_listener(std::any val, void *key);
    void call_listeners(std::any val);
    void clear();
    void remove_listener(void *key);
    virtual void stop() = 0;

signals:
    void message_error(QString);

public slots:
    virtual void start() = 0;    

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}

#endif // FRAME_CAPTURE_BASE_WORKER_HPP
