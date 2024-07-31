#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class widget_multi_stream_manager;

class QMessageBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

private:
    void action_about_qt(bool);
    void action_add_stream(bool);
    void action_contact_me(bool);
    void action_delete_stream(bool);
    void action_server_call();
    void action_warning(bool);

    void init_stacked_widget();

    void load_settings(bool);
    void save_settings(bool);

    void set_next_prev_button_visibility();

    Ui::MainWindow *ui;
            
    QMessageBox *msg_box_;

    widget_multi_stream_manager *widget_multi_stream_manager_;
};
