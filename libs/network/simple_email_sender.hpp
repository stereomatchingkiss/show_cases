#pragma once

#include <SimpleMail>

#include <QString>

#include <map>

namespace flt::net{

class simple_email_sender
{    
public:
    explicit simple_email_sender(QString const &email_address,
                                 QString const &password,
                                 QString const &sender_name,
                                 QString const &host = "smtp.gmail.com",
                                 quint16 port = 465);
    ~simple_email_sender();

    void add_send_to(QString const &email_address, QString const &recipient);
    void add_send_to(QStringList const &email_address, QStringList const &recipient);

    void remove_send_to(QString const &email_address);

    SimpleMail::ServerReply* send(std::vector<std::shared_ptr<SimpleMail::MimePart>> const &parts,
                                  QString const &subject = "Send by GenericCVTask");

private:
    SimpleMail::EmailAddress sender_;
    std::map<QString, QString> send_to_list_;
    SimpleMail::Server server_;
};

}
