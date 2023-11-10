#pragma once

#include <boost/circular_buffer.hpp>

#include <QObject>
#include <QPixmap>

#include <any>

namespace flt::mm{

class frame_process_base_worker : public QObject
{
    Q_OBJECT
public:
    explicit frame_process_base_worker(int buffer_size, QObject *parent = nullptr);
    ~frame_process_base_worker();

    void predict(std::any results);
    void stop(bool val);

public slots:    
    virtual void start();

signals:
    void message_error(QString);
    void send_process_results(std::any img);

private:
    std::any pop_front();
    virtual void process_results(std::any resutls) = 0;

    boost::circular_buffer<std::any> circular_buffer_;
    mutable std::mutex mutex_;
    std::atomic<bool> stop_;
};

}
