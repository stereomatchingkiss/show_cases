#pragma once

#include <QDialog>

namespace Ui {
class dialog_stream_select;
}

class dialog_stream_select : public QDialog
{
    Q_OBJECT
public:
    explicit dialog_stream_select(QWidget *parent = nullptr);
    ~dialog_stream_select();

    std::vector<QString> get_selected_streams() const;
    void set_streams(std::vector<QString> const &names);

private:
    Ui::dialog_stream_select *ui;
};
