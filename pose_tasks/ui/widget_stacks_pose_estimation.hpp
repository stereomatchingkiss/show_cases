#pragma once

#include <QWidget>

#include <any>

namespace Ui {
class widget_stacks_pose_estimation;
}

class widget_image_player;
class widget_pose_estimation_params;
class widget_source_selection;
class widget_stream_player;

namespace flt::mm{

class frame_process_controller;
class single_frame_with_multi_worker_base;

}

class widget_stacks_pose_estimation : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_pose_estimation(QWidget *parent = nullptr);
    ~widget_stacks_pose_estimation();

private slots:
    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

private:
    void init_stacked_widget();
    void next_page_is_pose_estimation_display();

    Ui::widget_stacks_pose_estimation *ui;

    widget_image_player *widget_image_player_;
    widget_pose_estimation_params *widget_pose_estimation_params_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;

    std::shared_ptr<flt::mm::frame_process_controller> process_controller_;
    std::unique_ptr<flt::mm::single_frame_with_multi_worker_base> sfwmw_;
};
