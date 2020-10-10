#include <QApplication>
#include "myBrew.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    myBrew b;
    b.show();

    return a.exec();
}
