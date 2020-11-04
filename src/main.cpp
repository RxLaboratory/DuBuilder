#include "mainwindow.h"
#include <QApplication>
#include <QtDebug>
#include "frameless.h"
#include "version.h"

#ifdef Q_OS_WIN
#include "windows.h"
#endif

class MessageHandler
{
public:
    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

void MessageHandler::messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
#ifdef QT_DEBUG
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
#endif
        break;
    case QtInfoMsg:
        fprintf(stdout, "%s\n", localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }
}

// Process the CLI arguments
bool processArgs(int argc, char *argv[])
{
    bool nobanner = false;
    bool help = false;

    // No console with a single arg on windows
#ifdef Q_OS_WIN
    bool hideConsole = argc < 3;
#endif

    for (int i = 1; i < argc; i++)
    {
        QString arg = argv[i];
        if ( arg.toLower() == "-n" || arg.toLower() == "--nobanner" ) nobanner = true;
        if (arg.toLower() == "-h" || arg.toLower() == "--help" ) help = true;
    }

    if (!nobanner)
    {
        qInfo().noquote() << STR_PRODUCTNAME;
        qInfo().noquote() << "version " + QString(STR_VERSION);
        qInfo().noquote() << STR_LEGALCOPYRIGHT;
        qInfo() << "";
        qInfo() << "This program comes with ABSOLUTELY NO WARRANTY;";
        qInfo() << "This is free software, and you are welcome to redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.";
        qInfo() << "";

#ifdef QT_DEBUG
        qInfo() << "Note: this is a development build and should not be used in production.";
        qInfo() << "";
#endif
    }

    if (help)
    {
#ifdef Q_OS_WIN
        hideConsole = false;
#endif
        qInfo() << "Usage: DuBuilder inputFile.jsx [options] [outputFile.jsx]";
        qInfo() << "";
        qInfo() << "Getting help";
        qInfo() << "    -h / --help                    Print basic options without launching DuBuilder";
        qInfo() << "    See the documentation at https://dubuilder-docs.rainboxlab.org for more detailed descriptions of the options";
        qInfo() << "";
        qInfo() << "Options";
        qInfo() << "    -n / --noBanner                 Hides the banner with copyright and license notice.";
        qInfo() << "    -d / --jsdoc jsdoc_conf.json    Also build the jsdoc using the jsdoc_conf.json configuration file. Jsdoc must be installed. See https://jsdoc.app for more info.";
    }

#ifdef Q_OS_WIN
#ifndef QT_DEBUG
    if (hideConsole)
    {
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    }
#endif
#endif

    return help;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // handles messages from the app and redirects them to stdout (info) or stderr (debug, warning, critical, fatal)
    qInstallMessageHandler(MessageHandler::messageOutput);

    bool stop = processArgs(argc, argv);

    if (stop) return 0;

    QCoreApplication::setOrganizationName("Rainbox Laboratory");
    QCoreApplication::setOrganizationDomain("rainboxlab.org");
    QCoreApplication::setApplicationName("DuBuilder");
    QCoreApplication::setApplicationVersion(STR_VERSION);
    MainWindow w(argc, argv);
    //the window is shown only if there's less than 2 args (the first one being the file to open)
    if (argc < 3)
    {
        FrameLess f(&w);
        w.show();
    }
    return a.exec();
}
