#ifndef WIDGET_ARUCO_CREATOR_HPP
#define WIDGET_ARUCO_CREATOR_HPP

#include <QImage>
#include <QWidget>

#include <vector>

namespace Ui {
class widget_aruco_creator;
}

class QTimer;

class widget_aruco_creator : public QWidget
{
    Q_OBJECT

public:
    explicit widget_aruco_creator(QWidget *parent = nullptr);
    ~widget_aruco_creator();

private slots:
    void on_pushButtonSaveAs_clicked();

    void on_comboBoxDictionary_currentIndexChanged(int index);

    void on_spinBoxMarkerID_valueChanged(int arg1);

    void on_spinBoxMarkerSize_valueChanged(int arg1);

    void on_spinBoxMarkerBorderBit_valueChanged(int arg1);

private:
    void generate_aruco_image(int id);
    void generate_aruco_image_time_up();
    void generate_aruco_option(int bit_size);
    void init_aruco_combo_box();
    void set_marker_range(int min, int max);

    Ui::widget_aruco_creator *ui;

    std::vector<int> aruco_size_;
    QImage qimg_aruco_;

    QTimer *timer_generate_aruco_;
};

#endif // WIDGET_ARUCO_CREATOR_HPP
