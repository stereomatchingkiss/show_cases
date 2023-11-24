#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <any>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class paddle_ocr_worker;

class widget_source_selection;
class widget_stream_player;

class QJsonObject;

namespace flt::mm{

class frame_process_controller;
class single_frame_with_multi_worker_base;

}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonNext_clicked();

    void on_pushButtonPrev_clicked();

private:
    void action_about_qt(bool);
    void action_contact_me(bool);
    void action_load_settings(bool);
    void action_save_settings(bool);
    void action_warning(bool);

    void display_frame(std::any val);

    QJsonObject dump_settings() const;

    void init_widgets_states(QJsonObject const &jobj);

    void save_settings_to_file(QString const &save_at) const;

    Ui::MainWindow *ui;   

    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;

    QSize origin_size_;
    std::shared_ptr<flt::mm::frame_process_controller> process_controller_;
    std::unique_ptr<flt::mm::single_frame_with_multi_worker_base> sfwmw_;
};
#endif // MAINWINDOW_HPP
