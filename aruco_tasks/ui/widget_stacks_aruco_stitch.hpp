#ifndef WIDGET_STACKS_ARUCO_STITCH_HPP
#define WIDGET_STACKS_ARUCO_STITCH_HPP

#include <QWidget>

namespace Ui {
class widget_stacks_aruco_stitch;
}

class dialog_aruco_detector_params;
class widget_source_selection;
class widget_stream_player;

namespace flt::mm{

class frame_process_controller;
class single_frame_with_multi_worker_base;

}

class widget_stacks_aruco_stitch : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_aruco_stitch(QWidget *parent = nullptr);
    ~widget_stacks_aruco_stitch();

private slots:
    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

private:
    void init_stacked_widget();

    Ui::widget_stacks_aruco_stitch *ui;

    dialog_aruco_detector_params *dialog_aruco_detector_params_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;

    std::shared_ptr<flt::mm::frame_process_controller> process_controller_;
    std::unique_ptr<flt::mm::single_frame_with_multi_worker_base> sfwmw_;
};

#endif // WIDGET_STACKS_ARUCO_STITCH_HPP
