#pragma once

#include <QObject>
#include <QPixmap>

#include <any>

namespace ocv::mm{

class frame_process_base_worker : public QObject
{
    Q_OBJECT
public:
    explicit frame_process_base_worker(QObject *parent = nullptr);
    ~frame_process_base_worker();

public slots:
    virtual void process_results(std::any resutls) = 0;

signals:
    void message_error(QString);
    void send_process_results(std::any img);
};

}
