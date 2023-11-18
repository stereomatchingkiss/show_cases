#ifndef DIALOG_DISPLAY_DETAILS_HPP
#define DIALOG_DISPLAY_DETAILS_HPP

#include <QDialog>

namespace Ui {
class dialog_display_details;
}

class QJsonObject;

struct config_dialog_display_details;

class dialog_display_details : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_display_details(QWidget *parent = nullptr);
    ~dialog_display_details();

    config_dialog_display_details get_config() const;

    QJsonObject get_states() const;
    void set_states(QJsonObject const &val);

private:
    Ui::dialog_display_details *ui;
};

#endif // DIALOG_DISPLAY_DETAILS_HPP
