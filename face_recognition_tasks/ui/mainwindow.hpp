#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QMessageBox;

class widget_stacks_manager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void action_about_qt(bool);
    void action_contact_me(bool);    
    void action_warning(bool);

    void init_stacked_widget();

    QJsonArray dump_stacks_states() const;
    QJsonObject dump_settings() const;
    void load_settings(bool);
    void init_widgets_states(QString const &fname);
    void init_widgets_states(QJsonObject const &jobj);
    void save_settings(bool);
    void save_settings_to_file(QString const &save_at) const;

    Ui::MainWindow *ui;

    QMessageBox *msg_box_;

    widget_stacks_manager *widget_stacks_manager_;
};
#endif // MAINWINDOW_HPP
