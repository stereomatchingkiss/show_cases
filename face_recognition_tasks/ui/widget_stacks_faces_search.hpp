#ifndef WIDGET_STACKS_FACES_SEARCH_HPP
#define WIDGET_STACKS_FACES_SEARCH_HPP

#include <QWidget>

namespace Ui {
class widget_stacks_faces_search;
}

namespace flt::mm{

class single_frame_with_multi_worker_base;

}

class widget_face_det_model_select;
class widget_face_recognition_model_select;
class widget_faces_register;
class widget_source_selection;
class widget_stream_player;

class widget_stacks_faces_search : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_faces_search(QWidget *parent = nullptr);
    ~widget_stacks_faces_search();

private slots:
    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

private:
    void create_frame_capture();
    void init_stacked_widget();
    void next_page_is_widget_stream_player();

    void send_msg_by_binary(QByteArray const &msg);
    void send_msg_by_text(QString const &msg);

    Ui::widget_stacks_faces_search *ui;

    std::unique_ptr<flt::mm::single_frame_with_multi_worker_base> sfwmw_;

    widget_face_det_model_select *widget_face_det_model_select_;
    widget_face_recognition_model_select *widget_face_recognition_model_select_;
    widget_faces_register *widget_faces_register_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player* widget_stream_player_;
};

#endif // WIDGET_STACKS_FACES_SEARCH_HPP
