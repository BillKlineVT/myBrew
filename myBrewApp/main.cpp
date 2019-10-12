#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include "myBrew.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    myBrew b;
    b.show();

    return a.exec();
}
