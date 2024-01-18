#ifndef WIDGET_STACKS_ARUCO_DETECTOR_HPP
#define WIDGET_STACKS_ARUCO_DETECTOR_HPP

#include <QWidget>

namespace Ui {
class widget_stacks_aruco_detector;
}

class dialog_aruco_detector_params;

class widget_stacks_aruco_detector : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_aruco_detector(QWidget *parent = nullptr);
    ~widget_stacks_aruco_detector();

signals:
    void prev_button_clicked();

private slots:
    void on_pushButtonPrev_clicked();

private:
    void init_stacked_widget();

    Ui::widget_stacks_aruco_detector *ui;

    dialog_aruco_detector_params *dialog_aruco_detector_params_;
};

#endif // WIDGET_STACKS_ARUCO_DETECTOR_HPP
