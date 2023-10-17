#ifndef FRAME_CAPTURE_BASE_WORKER_HPP
#define FRAME_CAPTURE_BASE_WORKER_HPP

#include <any>
#include <functional>
#include <memory>

#include <QObject>
#include <QString>

namespace ocv{

class frame_capture_base_worker : public QObject
{
    Q_OBJECT
public:
    explicit frame_capture_base_worker(QObject *parent = nullptr);

    virtual void add_listener(std::function<void(std::any)> func, void *key);
    virtual void clear();
    virtual void remove_listener(void *key);

signals:
    void message_error(QString);

public slots:
    virtual void start() = 0;
    virtual void stop() = 0;

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}

#endif // FRAME_CAPTURE_BASE_WORKER_HPP
