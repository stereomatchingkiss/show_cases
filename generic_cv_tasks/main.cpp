#include "ui/mainwindow.hpp"

#include "global/global_keywords.hpp"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDir().mkdir(global_keywords().cam_config_path());

    QCoreApplication::setOrganizationName("ThamSoft");
    QCoreApplication::setApplicationName("GenericCVTasks");

    MainWindow w;
    w.show();

    return a.exec();
}
