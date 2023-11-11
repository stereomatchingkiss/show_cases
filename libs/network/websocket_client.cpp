#include "websocket_client.hpp"

#include <QDebug>
#include <QWebSocket>

namespace flt::net{

struct websocket_client::impl
{
    QWebSocket socket_;
};

websocket_client::websocket_client(QObject *parent)
    : QObject{parent},
    impl_{std::make_unique<impl>()}
{
    connect(&impl_->socket_, &QWebSocket::connected, this, &websocket_client::connected);
    connect(&impl_->socket_, &QWebSocket::disconnected, this, &websocket_client::closed);
    connect(&impl_->socket_, &QWebSocket::errorOccurred, this, &websocket_client::socket_error);
}

websocket_client::~websocket_client()
{

}

void websocket_client::open(const QUrl &url)
{
    impl_->socket_.open(url);
}

#ifndef WASM_BUILD
void websocket_client::alert_received(QSsl::AlertLevel level, QSsl::AlertType type, const QString &description)
{
    qDebug()<<__func__<<"alert received. level = "
             <<static_cast<int>(level)<<", type = "<<static_cast<int>(type)<<", desc = "<<description;
}

void websocket_client::ssl_errors(const QList<QSslError> &errors)
{
    qDebug()<<__func__;
    for(auto const &val : errors){
        qDebug()<<"ssl errors : "<<val.errorString();
    }
}
#endif

void websocket_client::closed()
{
    qDebug()<<__func__<<":closed";
}

void websocket_client::connected()
{
    qDebug()<<__func__<<":connected";
}

void websocket_client::send_binary_message(QByteArray message)
{
    impl_->socket_.sendBinaryMessage(message);
}

void websocket_client::send_text_message(QString message)
{
    impl_->socket_.sendTextMessage(message);
}

void websocket_client::socket_error(QAbstractSocket::SocketError error)
{
    qDebug()<<__func__<<":"<<error;
}

void websocket_client::stop()
{
    impl_->socket_.close();
}

}
