#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class widget_source_selection;
class widget_stream_player;

class QJsonObject;
class QMessageBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
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

    QJsonObject dump_settings() const;

    void init_widgets_states(QJsonObject const &jobj);

    void save_settings_to_file(QString const &save_at) const;

    Ui::MainWindow *ui;

    QMessageBox *msg_box_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;
};
#endif // MAINWINDOW_HPP
