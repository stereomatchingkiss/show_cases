#ifndef WIDGET_STACKS_ANPR_HPP
#define WIDGET_STACKS_ANPR_HPP

#include <QWidget>

#include <any>

namespace Ui {
class widget_stacks_anpr;
}

class widget_object_detect_model_select;
class widget_stream_player;

namespace flt::mm{

class frame_process_controller;
class single_frame_with_multi_worker_base;

}

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
    void display_frame(std::any val);
    void init_stacked_widget();

    Ui::widget_stacks_anpr *ui;

    widget_object_detect_model_select *widget_object_detect_model_select_;    
    widget_stream_player *widget_stream_player_;

    std::shared_ptr<flt::mm::frame_process_controller> process_controller_;
    std::unique_ptr<flt::mm::single_frame_with_multi_worker_base> sfwmw_;
};

#endif // WIDGET_STACKS_ANPR_HPP
