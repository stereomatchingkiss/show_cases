#pragma once

#include <QAbstractSocket>
#include <QObject>

#ifndef WASM_BUILD
#include <QSslSocket>
#endif

#include <any>
#include <memory>

class QHostAddress;
class QWebSocket;

namespace flt::net{

class websocket_client : public QObject
{
    Q_OBJECT
public:
    explicit websocket_client(QObject *parent = nullptr);
    ~websocket_client();

    void open(QUrl const &url);

signals:
    void error_message(QString const &message);

private:
#ifndef WASM_BUILD
    void alert_received(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description);
    void ssl_errors(const QList<QSslError> &errors);
#endif    
    void closed();
    void connected();

    void send_binary_message(QByteArray message);
    void send_text_message(QString message);
    void socket_error(QAbstractSocket::SocketError error);
    void stop();    

    struct impl;
    std::unique_ptr<impl> impl_;

};

}
