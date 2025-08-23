#if defined(_MSC_VER)
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#endif

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}