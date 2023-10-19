#ifndef FRAME_PROCESS_CONTROLLER_HPP
#define FRAME_PROCESS_CONTROLLER_HPP

#include <QObject>
#include <QPixmap>

#include <any>

class QThread;

namespace ocv{

class frame_process_base_worker;

class frame_process_controller : public QObject
{
    Q_OBJECT
public:
    explicit frame_process_controller(frame_process_base_worker *worker, QObject *parent = nullptr);
    ~frame_process_controller();

signals:
    void message_error(QString);
    void process_frame(std::any frame);
    void send_frame_to_display(QPixmap img);

private:
    std::unique_ptr<QThread> thread_;
    frame_process_base_worker *worker_;
};

}

#endif // FRAME_PROCESS_CONTROLLER_HPP
