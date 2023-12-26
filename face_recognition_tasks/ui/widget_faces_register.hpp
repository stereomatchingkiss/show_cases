#ifndef WIDGET_FACES_REGISTER_HPP
#define WIDGET_FACES_REGISTER_HPP

#include "../algo/face_register/face_register_results.hpp"

#include <QWidget>

#include <any>

namespace Ui {
class widget_faces_register;
}

namespace flt::mm{

class frame_process_controller;

}

class QMessageBox;

class widget_faces_register : public QWidget
{
    Q_OBJECT

public:
    explicit widget_faces_register(QWidget *parent = nullptr);
    ~widget_faces_register();

    void clear_controller();

private slots:
    void on_pushButtonSelectImage_clicked();

    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

    void on_pushButtonAdd_clicked();

private:
    QJsonObject generate_save_content() const;
    void display_face();
    void init_controller();
    void process_results(std::any results);    

    Ui::widget_faces_register *ui;

    std::unique_ptr<flt::mm::frame_process_controller> controller_;
    size_t face_index_ = 0;
    face_register_results fresults_;

    QMessageBox *msg_box_;
};

#endif // WIDGET_FACES_REGISTER_HPP
