#include "mainwindow.h"
#include <QApplication>
#include "frameless.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Rainbox Laboratory");
    QCoreApplication::setOrganizationDomain("rainboxlab.org");
    QCoreApplication::setApplicationName("DuBuilder");
    QCoreApplication::setApplicationVersion(APPVERSION);
    MainWindow w;
    FrameLess f(&w);
    w.show();

    return a.exec();
}
