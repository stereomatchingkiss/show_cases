#ifndef WIDGET_ESTIMATE_SIMILAR_POSES_PLAYER_H
#define WIDGET_ESTIMATE_SIMILAR_POSES_PLAYER_H

#include <QWidget>

namespace Ui {
class widget_estimate_similar_poses_player;
}

class widget_estimate_similar_poses_player : public QWidget
{
    Q_OBJECT

public:
    explicit widget_estimate_similar_poses_player(QWidget *parent = nullptr);
    ~widget_estimate_similar_poses_player();

private:
    Ui::widget_estimate_similar_poses_player *ui;
};

#endif // WIDGET_ESTIMATE_SIMILAR_POSES_PLAYER_H
