#include "simple_email_sender.hpp"

using namespace SimpleMail;

namespace flt::net{

simple_email_sender::simple_email_sender(QString const &email_address,
                                         QString const &password,
                                         QString const &sender_name,
                                         QString const &host,
                                         quint16 port) :
    sender_{email_address, sender_name}
{
    server_.setHost(host);
    server_.setPort(port);
    server_.setConnectionType(Server::SslConnection);

    server_.setUsername(email_address.first(email_address.indexOf("@")));
    server_.setPassword(password);
}

simple_email_sender::~simple_email_sender()
{

}

void simple_email_sender::add_send_to(QString const &email_address, QString const &recipient)
{
    if(auto it = send_to_list_.find(email_address); it == std::end(send_to_list_)){
        send_to_list_.emplace(email_address, recipient);
    }else{
        it->second = recipient;
    }
}

void simple_email_sender::add_send_to(QStringList const &email_address, QStringList const &recipient)
{
    for(int i = 0; i != email_address.size(); ++i){
        add_send_to(email_address[i], recipient[i]);
    }
}

void simple_email_sender::remove_send_to(QString const &email_address)
{
    if(auto it = send_to_list_.find(email_address); it != std::end(send_to_list_)){
        send_to_list_.erase(it);
    }
}

SimpleMail::ServerReply *simple_email_sender::send(std::vector<std::shared_ptr<SimpleMail::MimePart>> const &parts,
                                                   QString const &subject)
{
    MimeMessage message;
    message.setSender(sender_);
    message.setSubject(subject);

    for(auto const &val : send_to_list_){
        message.addTo(EmailAddress(val.first, val.second));
    }
    for(auto const &val : parts){
        message.addPart(val);
    }

    return server_.sendMail(message);
}

void simple_email_sender::set_email_address(const QString &email_address)
{
    server_.setUsername(email_address.first(email_address.indexOf("@")));
    sender_.setAddress(email_address);
}

void simple_email_sender::set_password(const QString &password)
{
    server_.setPassword(password);
}

void simple_email_sender::set_sender_name(const QString &sender_name)
{
    sender_.setName(sender_name);
}

}
