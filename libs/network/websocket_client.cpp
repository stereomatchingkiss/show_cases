#include "websocket_client.hpp"

#include <QDebug>
#include <QWebSocket>

#include <mutex>

namespace flt::net{

struct websocket_client_thread_safe::impl
{
    std::mutex mutex_;
    QWebSocket socket_;
};

websocket_client_thread_safe::websocket_client_thread_safe(QObject *parent)
    : QObject{parent},
    impl_{std::make_unique<impl>()}
{
    connect(&impl_->socket_, &QWebSocket::connected, this, &websocket_client_thread_safe::connected);
    connect(&impl_->socket_, &QWebSocket::disconnected, this, &websocket_client_thread_safe::closed);
    connect(&impl_->socket_, &QWebSocket::errorOccurred, this, &websocket_client_thread_safe::socket_error);
}

websocket_client_thread_safe::~websocket_client_thread_safe()
{

}

void websocket_client_thread_safe::open(const QUrl &url)
{
    std::lock_guard<std::mutex> lock(impl_->mutex_);
    impl_->socket_.open(url);
}

#ifndef WASM_BUILD
void websocket_client_thread_safe::alert_received(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description)
{
    qDebug()<<__func__<<"alert received. level = "
             <<static_cast<int>(level)<<", type = "<<static_cast<int>(type)<<", desc = "<<description;
}

void websocket_client_thread_safe::ssl_errors(const QList<QSslError> &errors)
{
    qDebug()<<__func__;
    for(auto const &val : errors){
        qDebug()<<"ssl errors : "<<val.errorString();
    }
}
#endif

void websocket_client_thread_safe::closed()
{
    qDebug()<<__func__<<":closed";
}

void websocket_client_thread_safe::connected()
{
    qDebug()<<__func__<<":connected";
}

void websocket_client_thread_safe::send_binary_message(QByteArray message)
{
    std::lock_guard<std::mutex> lock(impl_->mutex_);
    impl_->socket_.sendBinaryMessage(message);
}

void websocket_client_thread_safe::send_text_message(QString message)
{
    std::lock_guard<std::mutex> lock(impl_->mutex_);
    impl_->socket_.sendTextMessage(message);
}

void websocket_client_thread_safe::socket_error(QAbstractSocket::SocketError error)
{
    qDebug()<<__func__<<":"<<error;
}

void websocket_client_thread_safe::stop()
{
    std::lock_guard<std::mutex> lock(impl_->mutex_);
    impl_->socket_.close();
}

}
