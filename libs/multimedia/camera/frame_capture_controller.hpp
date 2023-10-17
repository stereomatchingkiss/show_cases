#ifndef FRAME_CAPTURE_CONTROLLER_HPP_50454
#define FRAME_CAPTURE_CONTROLLER_HPP_50454

#include <any>
#include <functional>

#include <QObject>
#include <QString>

namespace ocv{

class frame_capture_base_worker;

class frame_capture_controller : public QObject
{
    Q_OBJECT
public:
    explicit frame_capture_controller(frame_capture_base_worker *worker, QObject *parent = nullptr);

    void add_listener(std::function<void(std::any)> func, void *key);
    void release();
    void remove_listener(void *key);

signals:    
    void message_error(QString);
    void start();
    void stop();

private:
    frame_capture_base_worker *worker_;
};

}

Q_DECLARE_METATYPE(std::function<void(std::any)>)

#endif
