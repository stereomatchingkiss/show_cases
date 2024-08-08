#ifndef DIALOG_EMAIL_LIST_HPP
#define DIALOG_EMAIL_LIST_HPP

#include <QDialog>

namespace Ui {
class dialog_email_list;
}

struct config_dialog_email_list;

class dialog_email_list : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_email_list(QWidget *parent = nullptr);
    ~dialog_email_list();

    config_dialog_email_list get_config() const;

    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

private slots:
    void on_buttonBox_accepted();

    void on_pushButtonAdd_clicked();

    void on_pushButtonDelete_clicked();

private:
    Ui::dialog_email_list *ui;
};

#endif // DIALOG_EMAIL_LIST_HPP
