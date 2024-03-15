#ifndef WIDGET_STREAM_PLAYER_HPP
#define WIDGET_STREAM_PLAYER_HPP

#include <QWidget>

#include <any>

namespace Ui {
class widget_stream_player;
}

class widget_stream_player : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stream_player(QWidget *parent = nullptr);
    ~widget_stream_player();

    void display_frame(std::any results);    

private:
    Ui::widget_stream_player *ui;    
};

#endif // WIDGET_STREAM_PLAYER_HPP
