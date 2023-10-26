#include "ui/mainwindow.hpp"

#include <QApplication>

#include <QDebug>
#include <QFile>
#include <QTextStream>

#include <set>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("ThamSoft");
    QCoreApplication::setApplicationName("GenericCVTasks");

    //MainWindow w;
    //w.show();

    std::set<QString> unique_list;
    if(QFile file("assets/faceexp-comparison-data-train-public.csv"); file.open(QIODevice::ReadOnly)){
        //int i = 0;
        QTextStream in(&file);
        while (!in.atEnd()){
            auto const slist = in.readLine().split(",");
            //qDebug()<<slist[0]<<", "<<slist[5]<<","<<slist[10];
            unique_list.insert(slist[0]);
            unique_list.insert(slist[5]);
            unique_list.insert(slist[10]);
            //++i;
            //if(i == 10){
            //    break;
            //}
        }
        qDebug()<<unique_list.size();
    }

    if(QFile file("unique_pulic_url.txt"); file.open(QIODevice::WriteOnly)){
        QTextStream stream(&file);
        for(auto const &val : unique_list){
            stream<<val.mid(1, val.size() - 2)<<"\n";
        }
    }

    return a.exec();
}
