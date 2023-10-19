#ifndef WIDGET_STREAM_PLAYER_HPP
#define WIDGET_STREAM_PLAYER_HPP

#include <QWidget>

#include <any>

namespace Ui {
class widget_stream_player;
}

namespace ocv{

class frame_capture_controller;
class frame_capture_opencv_worker;
class frame_process_controller;
class frame_process_base_worker;

struct frame_capture_params;

enum class stream_source_type;

}

class widget_stream_player : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stream_player(QWidget *parent = nullptr);
    ~widget_stream_player();

    /**
     * @param process_worker widget_stream_player will take care of the resource of the process worker
     */
    void play(ocv::frame_capture_params const &params, ocv::frame_process_base_worker *process_worker);

signals:
    void message_error(QString msg);
    void send_frame_to_display(QPixmap img);

private:
    void display_frame(QPixmap img);    

    Ui::widget_stream_player *ui;

    ocv::frame_capture_controller *frame_capture_controller_;
    ocv::frame_capture_opencv_worker *frame_capture_opencv_worker_;
    ocv::frame_process_controller *frame_process_controller_;
};

#endif // WIDGET_STREAM_PLAYER_HPP
