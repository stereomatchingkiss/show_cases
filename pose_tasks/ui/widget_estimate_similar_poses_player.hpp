#ifndef WIDGET_ESTIMATE_SIMILAR_POSES_PLAYER_H
#define WIDGET_ESTIMATE_SIMILAR_POSES_PLAYER_H

#include <QWidget>

#include <any>

namespace Ui {
class widget_estimate_similar_poses_player;
}

class QLabel;
struct estimate_many_pose_similarity_worker_results;

class widget_estimate_similar_poses_player : public QWidget
{
    Q_OBJECT

public:
    explicit widget_estimate_similar_poses_player(QWidget *parent = nullptr);
    ~widget_estimate_similar_poses_player();

    void clear_table();
    void display_frame(estimate_many_pose_similarity_worker_results const &input);
    void set_label_text(QString const &text);
    void set_request_image(QImage img);
    void set_similar_pose(std::any input);
    void set_similar_pose_visible(bool val);

signals:
    void fast_search_clicked(bool val);
    void image_selected(QString img);
    void similar_img_clicked(QString const &path);

private slots:
    void on_pushButtonSourceImage_clicked();

    void on_checkBoxFastSearch_clicked();

private:
    void cell_cliked(int row, int);
    void set_image(QImage const &qimg, QLabel *label);

    Ui::widget_estimate_similar_poses_player *ui;
};

#endif // WIDGET_ESTIMATE_SIMILAR_POSES_PLAYER_H
