#ifndef WIDGET_STREAM_PLAYER_HPP
#define WIDGET_STREAM_PLAYER_HPP

#include <QWidget>

#include <any>

namespace Ui {
class widget_stream_player;
}

namespace flt::mm{

enum class stream_source_type;

}

class widget_stream_player : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stream_player(QWidget *parent = nullptr);
    ~widget_stream_player();

    void display_frame(std::any results);
    void set_source_type(flt::mm::stream_source_type source_type);

signals:
    void image_selected(std::any img);
    void send_text_msg(QString msg);

private slots:
    void on_pushButtonSelectImage_clicked();

private:
    Ui::widget_stream_player *ui;    
};

#endif // WIDGET_STREAM_PLAYER_HPP
