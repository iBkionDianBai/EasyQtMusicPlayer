#include "Music.h"
#include "main.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString qss;
    QFile file(":/Qss/Theme1.qss");
    file.open(QFile::ReadOnly);
    qss = file.readAll();
    qInfo()<< qss;
    a.setStyleSheet(qss);

    Music w;
    w.show();
    return a.exec();
}
