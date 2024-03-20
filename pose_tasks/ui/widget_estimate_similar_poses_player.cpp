#include "widget_estimate_similar_poses_player.hpp"
#include "ui_widget_estimate_similar_poses_player.h"

widget_estimate_similar_poses_player::widget_estimate_similar_poses_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_estimate_similar_poses_player)
{
    ui->setupUi(this);
}

widget_estimate_similar_poses_player::~widget_estimate_similar_poses_player()
{
    delete ui;
}
