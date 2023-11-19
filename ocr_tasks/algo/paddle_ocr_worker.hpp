#pragma once

#include <multimedia/camera/frame_process_base_worker.hpp>

#include <memory>

struct config_paddle_ocr_worker;

class paddle_ocr_worker  : public flt::mm::frame_process_base_worker
{
    Q_OBJECT
public:
    explicit paddle_ocr_worker(config_paddle_ocr_worker const &params, QObject *parent = nullptr);
    ~paddle_ocr_worker();

    void process_results(std::any frame) override;

signals:
    void send_alert_by_binary(QByteArray msg);
    void send_alert_by_text(QString msg);

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};
