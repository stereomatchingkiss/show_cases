#include "widget_image_pair_player.hpp"
#include "ui_widget_image_pair_player.h"

widget_image_pair_player::widget_image_pair_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_image_pair_player)
{
    ui->setupUi(this);
}

widget_image_pair_player::~widget_image_pair_player()
{
    delete ui;
}

void widget_image_pair_player::on_pushButtonSourceImage_clicked()
{

}


void widget_image_pair_player::on_pushButtonTargetImage_clicked()
{

}

