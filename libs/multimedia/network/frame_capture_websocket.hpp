#pragma once

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

class frame_capture_websocket : public QObject
{
    Q_OBJECT
public:
    explicit frame_capture_websocket(frame_capture_websocket_params const &params);
    ~frame_capture_websocket();

    void add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key);
    void remove_listener(void *key);
    void start();    

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

    struct impl;
    std::unique_ptr<impl> impl_;
};

}
