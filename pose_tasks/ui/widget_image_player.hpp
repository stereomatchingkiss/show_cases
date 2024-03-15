#ifndef WIDGET_IMAGE_PLAYER_HPP
#define WIDGET_IMAGE_PLAYER_HPP

#include <QImage>
#include <QWidget>

#include <any>

namespace Ui {
class widget_image_player;
}

class widget_image_player : public QWidget
{
    Q_OBJECT

public:
    explicit widget_image_player(QWidget *parent = nullptr);
    ~widget_image_player();

    void call_image_selected();

    void display_frame(std::any results);

signals:    
    void image_selected(std::any img);    

private slots:
    void on_pushButtonSelectImage_clicked();

private:
    Ui::widget_image_player *ui;

    QImage origin_img_;
};

#endif // WIDGET_IMAGE_PLAYER_HPP
