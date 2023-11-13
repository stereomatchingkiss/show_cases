#pragma once

#include <QAbstractSocket>
#include <QObject>

#ifndef WASM_BUILD
#include <QSslSocket>
#endif

#include <memory>

class QHostAddress;
class QThread;
class QWebSocket;

namespace flt::net{

class websocket_client_worker;

class websocket_client_controller : public QObject
{
    Q_OBJECT
public:
    explicit websocket_client_controller(QObject *parent = nullptr);
    ~websocket_client_controller();

signals:
    void close();
    void initialize();
    void open(QUrl const &url);
    void reopen_if_needed(QUrl const &url);
    void send_binary_message(QByteArray message);
    void send_text_message(QString message);

private:
    std::unique_ptr<QThread> thread_;
    websocket_client_worker *worker_;
};

}
