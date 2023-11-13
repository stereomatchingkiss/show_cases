#include "websocket_client_worker.hpp"

#include <QDebug>
#include <QWebSocket>

#include <mutex>

namespace flt::net{

struct websocket_client_worker::impl
{    
    QWebSocket socket_;
};

websocket_client_worker::websocket_client_worker(QObject *parent)
    : QObject{parent},
    impl_{std::make_unique<impl>()}
{    
}

websocket_client_worker::~websocket_client_worker()
{

}

void websocket_client_worker::close()
{
    impl_->socket_.close();
}

void websocket_client_worker::create_connection()
{
    disconnect(&impl_->socket_, &QWebSocket::connected, this, &websocket_client_worker::connected);
    disconnect(&impl_->socket_, &QWebSocket::disconnected, this, &websocket_client_worker::closed);
    disconnect(&impl_->socket_, &QWebSocket::errorOccurred, this, &websocket_client_worker::socket_error);

    connect(&impl_->socket_, &QWebSocket::connected, this, &websocket_client_worker::connected);
    connect(&impl_->socket_, &QWebSocket::disconnected, this, &websocket_client_worker::closed);
    connect(&impl_->socket_, &QWebSocket::errorOccurred, this, &websocket_client_worker::socket_error);
}

void websocket_client_worker::open(const QUrl &url)
{    
    impl_->socket_.open(url);
}

void websocket_client_worker::reconnect_if_needed(const QUrl &url)
{
    qDebug()<<"reconnect = "<<impl_->socket_.state();
    if(impl_->socket_.state() != QAbstractSocket::ConnectedState){
        impl_->socket_.close();
        impl_->socket_.open(url);
    }
}

#ifndef WASM_BUILD
void websocket_client_worker::alert_received(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description)
{
    qDebug()<<__func__<<"alert received. level = "
             <<static_cast<int>(level)<<", type = "<<static_cast<int>(type)<<", desc = "<<description;
}

void websocket_client_worker::ssl_errors(const QList<QSslError> &errors)
{
    qDebug()<<__func__;
    for(auto const &val : errors){
        qDebug()<<"ssl errors : "<<val.errorString();
    }
}
#endif

void websocket_client_worker::closed()
{
    qDebug()<<__func__<<":closed";
}

void websocket_client_worker::connected()
{
    qDebug()<<__func__<<":connected";
}

void websocket_client_worker::send_binary_message(QByteArray message)
{    
    impl_->socket_.sendBinaryMessage(message);
}

void websocket_client_worker::send_text_message(QString message)
{    
    impl_->socket_.sendTextMessage(message);
}

void websocket_client_worker::socket_error(QAbstractSocket::SocketError error)
{
    qDebug()<<__func__<<":"<<error;
}

}
