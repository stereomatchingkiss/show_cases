#include "websocket_server_base.hpp"

#include <QFile>

#include <QSslConfiguration>
#include <QSslKey>
#include <QWebSocket>
#include <QWebSocketServer>

namespace ftl::net{

websocket_server_base::websocket_server_base(QString const &server_name, QObject *parent)
    : QObject(parent),
      server_name_(server_name)
{    
}

bool websocket_server_base::listen(const QHostAddress &address, quint16 port)
{
    create_server();

    return listen_to(address, port);
}

bool websocket_server_base::listen_to_ipv4(quint16 port)
{
    return listen(QHostAddress::AnyIPv4, port);
}

bool websocket_server_base::listen(QHostAddress const &address,
                                   quint16 port,
                                   QString const &cert_file,
                                   QString const &key_file)
{
    create_server(cert_file, key_file);

    return listen_to(address, port);
}

bool websocket_server_base::listen_to_ipv4(quint16 port,
                                           const QString &cert_file,
                                           const QString &key_file)
{
    return listen(QHostAddress::AnyIPv4, port, cert_file, key_file);
}

bool websocket_server_base::send_binary_message(const QByteArray &input, QWebSocket *socket)
{
    if(auto it = sockets_.find(socket); it != std::end(sockets_)){
        socket->sendBinaryMessage(input);
        return true;
    }

    return false;
}

bool websocket_server_base::send_text_message(const QString &input, QWebSocket *socket)
{
    if(auto it = sockets_.find(socket); it != std::end(sockets_)){
        socket->sendTextMessage(input);
        return true;
    }

    return false;
}

void websocket_server_base::send_binary_message(const QByteArray &input)
{
    if(server_){
        for(auto *soc: sockets_){
            soc->sendBinaryMessage(input);
        }
    }
}

void websocket_server_base::send_text_message(const QString &input)
{
    if(server_){
        for(auto *soc : sockets_){
            soc->sendTextMessage(input);
        }
    }
}

bool websocket_server_base::server_on() const noexcept
{
    return server_ != nullptr;
}

void websocket_server_base::client_connected()
{
    if(server_){
        socket_type *client_connection = server_->nextPendingConnection();

        qDebug()<<__func__<<client_connection->peerAddress()<<","
                  <<client_connection->peerPort()<<","
                 <<client_connection->peerName();
        connect(client_connection, &socket_type::disconnected,
                this, &websocket_server_base::delete_socket);
        connect(client_connection, &socket_type::errorOccurred,
                this, &websocket_server_base::socket_error);

        connect(client_connection, &socket_type::binaryMessageReceived,
                this, &websocket_server_base::binary_message_received);
        connect(client_connection, &socket_type::textMessageReceived,
                this, &websocket_server_base::text_message_received);
        connect(client_connection, &socket_type::binaryMessageReceived,
                this, &websocket_server_base::process_binary_message_received_from_the_client);
        connect(client_connection, &socket_type::textMessageReceived,
                this, &websocket_server_base::process_text_message_received_from_the_client);
        sockets_.insert(client_connection);
    }
}

void websocket_server_base::create_server()
{
    if(!server_){
        server_ = new server_type(server_name_,
                                  QWebSocketServer::NonSecureMode,
                                  this);
    }
}

void websocket_server_base::create_server(const QString &cert_file, const QString &key_file)
{
    if(!server_){
        server_ = new server_type(server_name_,
                                  QWebSocketServer::SecureMode,
                                  this);

        QSslConfiguration sslConfiguration;
        QFile certFile(cert_file);
        QFile keyFile(key_file);
        certFile.open(QIODevice::ReadOnly);
        keyFile.open(QIODevice::ReadOnly);
        QSslCertificate certificate(&certFile, QSsl::Pem);
        QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
        certFile.close();
        keyFile.close();
        sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
        sslConfiguration.setLocalCertificate(certificate);
        sslConfiguration.setPrivateKey(sslKey);
        sslConfiguration.setProtocol(QSsl::TlsV1_3OrLater);
        server_->setSslConfiguration(sslConfiguration);
    }
}

void websocket_server_base::close()
{
    qDebug()<<__func__;
    if(server_){
        server_->close();
        server_->deleteLater();
        server_ = nullptr;
    }
}

void websocket_server_base::delete_socket()
{
    qDebug()<<__func__;
    if(auto socket = qobject_cast<socket_type*>(sender()); socket){
        QString const peer_addr = socket->peerAddress().toString();
        qDebug()<<__func__<<"delete socket addr:"<<peer_addr;
        if(auto it = sockets_.find(socket); it != std::end(sockets_)){
            qDebug()<<__func__<<"close socket addr";
            socket->close();
            sockets_.erase(socket);
            qDebug()<<__func__<<"delete socket later";
            socket->deleteLater();
        }
    }
}

bool websocket_server_base::listen_to(const QHostAddress &address, quint16 port)
{
    if(!server_){
        return false;
    }
    if(server_->isListening()){
        if(server_->serverAddress() == address && server_->serverPort() == port){
            return true;
        }
    }
    if(server_->listen(address, port)){
        qInfo()<<__func__<<", server listen to addr:"<<server_->serverAddress().toString()
                  <<", port:"<<server_->serverPort()<<" end";
        connect(server_, &server_type::newConnection, this, &websocket_server_base::client_connected);
        connect(server_, &server_type::sslErrors, this, &websocket_server_base::ssl_errors);
        return true;
    }else{
        emit error_message(server_->errorString());
        return false;
    }
}

void websocket_server_base::process_binary_message_received_from_the_client(const QByteArray &message)
{    
    if(auto *socket = qobject_cast<QWebSocket*>(sender()); socket){
        emit binary_message_received_from_the_client(message, socket);
    }else{
        qDebug()<<__func__<<"cannot cast to websocket";
    }
}

void websocket_server_base::process_text_message_received_from_the_client(const QString &message)
{    
    if(auto *socket = qobject_cast<QWebSocket*>(sender()); socket){
        emit text_message_received_from_the_client(message, socket);
    }else{
        qDebug()<<__func__<<"cannot cast to websocket";
    }
}

void websocket_server_base::socket_error(QAbstractSocket::SocketError error)
{
    qDebug()<<__func__<<error;
    auto socket = qobject_cast<socket_type*>(sender());
    if(socket){
        qDebug()<<__func__<<"socket ip, port = "
                   <<socket->peerAddress().toString()
                  <<socket->peerPort()<<","<<error;
    }else{
        qDebug()<<__func__<<error;
    }
}

void websocket_server_base::ssl_errors(const QList<QSslError> &errors)
{
    qDebug()<<__func__<<errors;
}


}
