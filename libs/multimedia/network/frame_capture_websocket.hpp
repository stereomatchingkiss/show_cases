#pragma once

#include "../camera/single_frame_with_multi_worker_base.hpp"

#include <QAbstractSocket>
#include <QObject>

#ifndef WASM_BUILD
#include <QSslSocket>
#endif

#include <any>
#include <memory>

class QHostAddress;
class QWebSocket;

namespace flt::mm{

class frame_process_controller;

struct frame_capture_websocket_params;

class frame_capture_websocket : public single_frame_with_multi_worker_base
{
    Q_OBJECT
public:
    explicit frame_capture_websocket(frame_capture_websocket_params const &params, QObject *parent = nullptr);
    ~frame_capture_websocket();

    void add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key) override;
    void remove_listener(void *key) override;
    void start() override;

    void send_text_message(QString msg);

signals:    
    void error_message(QString const &message);

private:
#ifndef WASM_BUILD
    void alert_received(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description);
    void ssl_errors(const QList<QSslError> &errors);
#endif
    void binary_message_received(QByteArray message);
    void closed();
    void connected();

    void socket_error(QAbstractSocket::SocketError error);
    void stop();
    void text_message_received(QString message);

    void json_binary_message_received(QByteArray message);
    void json_text_message_received(QString message);

    struct impl;
    std::unique_ptr<impl> impl_;
};

}
