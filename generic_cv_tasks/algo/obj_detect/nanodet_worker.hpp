#pragma once

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <memory>

struct config_alert_sender;
struct config_nanodet_worker;

class nanodet_worker : public flt::mm::frame_process_base_worker
{
    Q_OBJECT
public:
    explicit nanodet_worker(config_nanodet_worker config, QObject *parent = nullptr);
    ~nanodet_worker();

    void change_alert_sender_config(config_alert_sender const &val);    

signals:
    void send_alert_by_binary(QByteArray msg);
    void send_alert_by_text(QString msg);

private:
    void process_results(std::any frame) override;

    struct impl;
    std::unique_ptr<impl> impl_;
};
