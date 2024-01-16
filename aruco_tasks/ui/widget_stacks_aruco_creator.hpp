#ifndef WIDGET_STACKS_ARUCO_CREATOR_HPP
#define WIDGET_STACKS_ARUCO_CREATOR_HPP

#include <QWidget>

namespace Ui {
class widget_stacks_aruco_creator;
}

class widget_aruco_creator;

class widget_stacks_aruco_creator : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_aruco_creator(QWidget *parent = nullptr);
    ~widget_stacks_aruco_creator();

signals:
    void prev_button_clicked();

private slots:
    void on_pushButton_clicked();

private:
    void init_stacked_widget();

    Ui::widget_stacks_aruco_creator *ui;

    widget_aruco_creator *widget_aruco_creator_;
};

#endif // WIDGET_STACKS_ARUCO_CREATOR_HPP
