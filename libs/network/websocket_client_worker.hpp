#pragma once

#include <QAbstractSocket>
#include <QObject>

#ifndef WASM_BUILD
#include <QSslSocket>
#endif

#include <memory>

class QHostAddress;
class QWebSocket;

namespace flt::net{

class websocket_client_worker : public QObject
{
    Q_OBJECT
public:
    explicit websocket_client_worker(QObject *parent = nullptr);
    ~websocket_client_worker();

    void close();
    void create_connection();
    void open(QUrl const &url);
    void restart_if_needed(QUrl const &url);
    void send_binary_message(QByteArray message);
    void send_text_message(QString message);    

private:
#ifndef WASM_BUILD
    void alert_received(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description);
    void ssl_errors(const QList<QSslError> &errors);
#endif    
    void closed();
    void connected();

    void socket_error(QAbstractSocket::SocketError error);

    struct impl;
    std::unique_ptr<impl> impl_;
};

}
