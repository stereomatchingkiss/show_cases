#ifndef WIDGET_STACKS_ANPR_HPP
#define WIDGET_STACKS_ANPR_HPP

#include <QWidget>

namespace Ui {
class widget_stacks_anpr;
}

class widget_object_detect_model_select;
class widget_source_selection;

class widget_stacks_anpr : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_anpr(QWidget *parent = nullptr);
    ~widget_stacks_anpr();

private slots:
    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

private:
    void init_stacked_widget();

    Ui::widget_stacks_anpr *ui;

    widget_object_detect_model_select *widget_object_detect_model_select_;
    widget_source_selection *widget_source_selection_;
};

#endif // WIDGET_STACKS_ANPR_HPP
