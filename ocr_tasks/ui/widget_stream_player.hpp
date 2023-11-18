#ifndef WIDGET_STREAM_PLAYER_HPP
#define WIDGET_STREAM_PLAYER_HPP

#include <QWidget>

#include <any>

namespace Ui {
class widget_stream_player;
}

class dialog_display_details;

class widget_stream_player : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stream_player(QWidget *parent = nullptr);
    ~widget_stream_player();

    void display_frame(std::any results);

    void set_can_save_on_local(bool val);

signals:
    void image_selected(std::any img);    

private slots:
    void on_pushButtonSave_clicked();

    void on_pushButtonSelectImage_clicked();

    void on_pushButtonDisplayDetails_clicked();

private:
    Ui::widget_stream_player *ui;    

    bool can_save_on_local_ = true;
    dialog_display_details *dialog_display_details_;
};

#endif // WIDGET_STREAM_PLAYER_HPP
