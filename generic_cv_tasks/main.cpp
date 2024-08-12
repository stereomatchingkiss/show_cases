#include "ui/mainwindow.hpp"

#include "config/config_alert_sender.hpp"

#include "global/global_keywords.hpp"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<config_alert_sender>("config_alert_sender");

    QDir().mkpath(global_keywords().action_classify_alert_path());
    QDir().mkpath(global_keywords().cam_config_path());
    QDir().mkpath(global_keywords().tracker_alert_path());

    QCoreApplication::setOrganizationName("ThamSoft");
    QCoreApplication::setApplicationName("GenericCVTasks");

    MainWindow w;
    w.show();

    return a.exec();
}
