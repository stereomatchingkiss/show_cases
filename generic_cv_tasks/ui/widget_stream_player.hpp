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

    void set_is_seekable(bool val);
    void set_current_position(qint64 val);
    void set_duration(qint64 current_duration_msec, qint64 max_duration_msec);    

signals:
    void play();    
    void pause();
    void seek(int msec);

private:
    Ui::widget_stream_player *ui;

    QString format_;
    bool playing_;
};

#endif // WIDGET_STREAM_PLAYER_HPP
