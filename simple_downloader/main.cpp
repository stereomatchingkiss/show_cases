#include "mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("ThamSoft");
    QCoreApplication::setApplicationName("SimpleDownloader");

    MainWindow w;
    w.show();

    return a.exec();
}
