#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowState(w.windowState() | Qt::WindowMaximized);
    w.show();
    return a.exec();
}
