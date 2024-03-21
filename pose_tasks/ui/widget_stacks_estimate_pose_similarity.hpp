#ifndef WIDGET_STACKS_ESTIMATE_POSE_SIMILARITY_HPP
#define WIDGET_STACKS_ESTIMATE_POSE_SIMILARITY_HPP

#include <QWidget>

#include <any>

namespace Ui {
class widget_stacks_estimate_pose_similarity;
}

class widget_estimate_many_pose_similarity_params;
class widget_estimate_similar_poses_player;
class widget_image_pair_player;
class widget_pose_estimation_params;
class widget_source_selection;

namespace flt::mm{

class frame_process_controller;
class single_frame_with_multi_worker_base;

}

class widget_stacks_estimate_pose_similarity : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_estimate_pose_similarity(QWidget *parent = nullptr);
    ~widget_stacks_estimate_pose_similarity();

signals:
    void prev_button_clicked();

private slots:
    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

private:
    void image_selected(QString info);
    void init_stacked_widget();
    void next_page_is_estimate_pose_similarity_display();
    void process_source_image(QImage img);
    void process_target_image(QImage img);
    void received_process_msg(QString msg);
    void similar_img_clicked(QString const &path);

    Ui::widget_stacks_estimate_pose_similarity *ui;

    widget_estimate_many_pose_similarity_params *widget_estimate_many_pose_similarity_params_;
    widget_estimate_similar_poses_player *widget_estimate_similar_poses_player_;
    widget_image_pair_player *widget_image_pair_player_;
    widget_pose_estimation_params *widget_pose_estimation_params_;
    widget_source_selection *widget_source_selection_;

    std::shared_ptr<flt::mm::frame_process_controller> process_controller_;
    std::unique_ptr<flt::mm::single_frame_with_multi_worker_base> sfwmw_;
};

#endif // WIDGET_STACKS_ESTIMATE_POSE_SIMILARITY_HPP
