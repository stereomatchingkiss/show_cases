#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class widget_stacks_manager;

class QMessageBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void action_about_qt(bool);
    void action_contact_me(bool);
    void action_readme(bool);

    void init_stacked_widget();

    Ui::MainWindow *ui;

    QMessageBox *msg_box_;
    widget_stacks_manager *widget_stacks_manager_;
};
#endif // MAINWINDOW_HPP
