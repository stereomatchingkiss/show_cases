#include "ui/mainwindow.hpp"

#include "global/global_keywords.hpp"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDir().mkpath(global_keywords().cam_config_path());

    MainWindow w;
    w.show();

    return a.exec();
}
