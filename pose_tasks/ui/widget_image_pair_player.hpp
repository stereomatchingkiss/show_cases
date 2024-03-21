#ifndef WIDGET_IMAGE_PAIR_PLAYER_HPP
#define WIDGET_IMAGE_PAIR_PLAYER_HPP

#include <QImage>
#include <QWidget>

#include <any>

namespace Ui {
class widget_image_pair_player;
}

class QLabel;

class widget_image_pair_player : public QWidget
{
    Q_OBJECT

public:
    explicit widget_image_pair_player(QWidget *parent = nullptr);
    ~widget_image_pair_player();

    void display_frame(std::any input);

signals:
    void source_image_selected(QImage img);
    void target_image_selected(QImage img);

private slots:
    void on_pushButtonSourceImage_clicked();

    void on_pushButtonTargetImage_clicked();

private:
    void load_image(QImage &output, bool is_source_image);
    void set_image(QImage const &img, QLabel *label);

    Ui::widget_image_pair_player *ui;

    QImage source_img_;
    QImage target_img_;
};

#endif // WIDGET_IMAGE_PAIR_PLAYER_HPP
