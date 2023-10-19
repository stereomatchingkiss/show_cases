#ifndef FRAME_PROCESS_BASE_WORKER_HPP
#define FRAME_PROCESS_BASE_WORKER_HPP

#include <QObject>
#include <QPixmap>

#include <any>

namespace ocv{

class frame_process_base_worker : public QObject
{
    Q_OBJECT
public:
    explicit frame_process_base_worker(QObject *parent = nullptr);
    ~frame_process_base_worker();

public slots:
    virtual void process_frame(std::any frame) = 0;

signals:
    void message_error(QString);
    void send_frame_to_display(QPixmap img);
};

}

#endif // FRAME_PROCESS_BASE_WORKER_HPP
