#ifndef WIDGET_ESTIMATE_SIMILAR_POSES_PLAYER_H
#define WIDGET_ESTIMATE_SIMILAR_POSES_PLAYER_H

#include <QWidget>

#include <any>

namespace Ui {
class widget_estimate_similar_poses_player;
}

class widget_estimate_similar_poses_player : public QWidget
{
    Q_OBJECT

public:
    explicit widget_estimate_similar_poses_player(QWidget *parent = nullptr);
    ~widget_estimate_similar_poses_player();

    void display_frame(std::any input);
    void set_label_text(QString const &text);
    void set_similar_pose(std::any input);

signals:
    void image_selected(QString img);

private slots:
    void on_pushButtonSourceImage_clicked();

private:
    Ui::widget_estimate_similar_poses_player *ui;
};

#endif // WIDGET_ESTIMATE_SIMILAR_POSES_PLAYER_H
