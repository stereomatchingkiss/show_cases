#ifndef WEBSOCKET_SERVER_BASE_HPP
#define WEBSOCKET_SERVER_BASE_HPP

#include <QAbstractSocket>
#include <QWebSocketServer>
#include <QObject>

#include <set>

class QHostAddress;
class QWebSocket;
class QWebSocketServer;

namespace ftl::net{

class websocket_server_base : public QObject
{
    Q_OBJECT
public:
    explicit websocket_server_base(QString const &server_name, QObject *parent = nullptr);

    virtual void close();
    /**
     * Listen to specific address with port value
     */
    virtual bool listen(QHostAddress const &address, quint16 port);
    virtual bool listen_to_ipv4(quint16 port);
    virtual bool listen(QHostAddress const &address, quint16 port,
                        QString const &cert_file,
                        QString const &key_file);
    virtual bool listen_to_ipv4(quint16 port,
                                QString const &cert_file,
                                QString const &key_file);

    bool send_binary_message(QByteArray const &input, QWebSocket *socket);
    bool send_text_message(QString const &input, QWebSocket *socket);
    virtual void send_binary_message(QByteArray const &input);    
    virtual void send_text_message(QString const &input);

signals:
    void binary_message_received(QByteArray const &message);
    void binary_message_received_from_the_client(QByteArray const &message, QWebSocket *socket);
    void error_message(QString const &message);
    void text_message_received(QString const &message);
    void text_message_received_from_the_client(QString const &message, QWebSocket *socket);

protected:
    bool server_on() const noexcept;

private:
    using server_type = QWebSocketServer;
    using socket_type = QWebSocket;

    void client_connected();
    void create_server();
    void create_server(QString const &cert_file, QString const &key_file);
    void delete_socket();
    bool listen_to(QHostAddress const &address, quint16 port);

    void process_binary_message_received_from_the_client(QByteArray const &message);
    void process_text_message_received_from_the_client(QString const &message);

    void socket_error(QAbstractSocket::SocketError error);
    void ssl_errors(const QList<QSslError> &errors);


    std::set<socket_type*> sockets_;
    server_type *server_ = nullptr;
    QString server_name_;
};

} //namespace enetwork

#endif // WEBSOCKET_SERVER_BASE_HPP
