#include "mainwindow.h"

#include <QApplication>
#include <QSettings>

#ifdef Q_OS_WIN
#include "windows.h"
#endif

#include "duqf-app/app-utils.h"

int main(int argc, char *argv[])
{
    DuApplication a(argc, argv);

    // process CLI arguments
    if ( processArgs(argc, argv) ) return 0;

    // settings
    QCoreApplication::setOrganizationName(STR_COMPANYNAME);
    QCoreApplication::setOrganizationDomain(STR_COMPANYDOMAIN);
    QCoreApplication::setApplicationName(STR_PRODUCTNAME);
    QCoreApplication::setApplicationVersion(STR_VERSION);

    //the window is shown only if there's less than 2 args (the first one being the file to open)
    if (argc < 3)
    {
        // show splashscreen
        a.showSplashScreen();
        DuSplashScreen *s = a.splashScreen();

        // build and show UI
        s->newMessage("Building UI");
        MainWindow *w = new MainWindow( argc, argv );

#ifndef Q_OS_LINUX
        FrameLessWindow f(w);
#endif
        w->show();

        // hide splash when finished
        s->finish(w);
    }
    return a.exec();
}
