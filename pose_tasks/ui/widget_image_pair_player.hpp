#ifndef WIDGET_IMAGE_PAIR_PLAYER_HPP
#define WIDGET_IMAGE_PAIR_PLAYER_HPP

#include <QWidget>

namespace Ui {
class widget_image_pair_player;
}

class widget_image_pair_player : public QWidget
{
    Q_OBJECT

public:
    explicit widget_image_pair_player(QWidget *parent = nullptr);
    ~widget_image_pair_player();

private slots:
    void on_pushButtonSourceImage_clicked();

    void on_pushButtonTargetImage_clicked();

private:
    Ui::widget_image_pair_player *ui;
};

#endif // WIDGET_IMAGE_PAIR_PLAYER_HPP
