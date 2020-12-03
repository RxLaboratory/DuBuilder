#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include "duqf-app/app-version.h"
#include "duqf-app/app-style.h"
#include "duqf-widgets/toolbarspacer.h"
#include "duqf-widgets/settingswidget.h"

#include <QFileDialog>
#include <QFile>
#include <QMouseEvent>
#include <QSettings>
#include <QMenu>
#include <QToolButton>
#include <QMessageBox>
#include <QDesktopServices>
#include <QLabel>
#include <QtDebug>

#include "scanner.h"
#include "builder.h"
#include "scriptwidget.h"
#include "includesettingswidget.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int argc, char *argv[], QWidget *parent = nullptr);

private slots:
    void scanned(Script *script);
    void built(QString builtScript);
    void progress(int i, QString message);
    void jsdocBuilt(int exitCode, QProcess::ExitStatus exitStatus);
    void jsdocOutput();
    void jsdocError(QProcess::ProcessError error);
    //actions
    void on_actionOpen_Script_triggered();
    void on_actionRe_scan_script_triggered();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_actionBuild_triggered();
    void on_actionBuild_JSDoc_triggered(bool checked);
    void removeCurrentIncludeItems();

    //UI
    /**
     * @brief Sets the UI in waiting mode, when long operations are going on
     * @param wait False to stop waiting mode
     */
    void setWaiting(bool wait = true);

    void duqf_maximize(bool max);
    void duqf_maximize();
    void duqf_bugReport();
    void duqf_forum();
    void duqf_chat();
    void duqf_doc();
    void duqf_settings(bool checked = true);
    void duqf_closeSettings();

private:

    //WORKER THREADS

    Scanner *scanner;
    Builder *builder;
    QProcess *jsdocProcess;

    //CURRENT

    /**
     * @brief The script currently opened
     */
    Script *currentScript;
    /**
     * @brief The path of the script currently building
     * This is the file where it will be written in the built() slot
     */
    QString savePath;
    /**
     * @brief scanningItem The item being scanned. nullptr if root
     */
    QTreeWidgetItem *scanningItem;
    QFileInfo jsdocConfFile;
    /**
     * @brief outputFile The path of the output file, if provided by command line args
     */
    QString outputFile;

    //METHODS

    void mapEvents();
    bool openFile(QString filePath);
    bool buildFile(QString filePath);
    bool updateScript(Script *containingScript, Script *newScript);
    void removeInclude(Script *containingScript, int scriptId);
    void removeInclude(Script *containingScript, Script *includeScript);
    QTreeWidgetItem *createIncludeItem(Script *script);

    //UI
    QToolButton* buildMenuButton;
    QToolButton* helpMenuButton;

    // ========= RxOT UI ==============
    /**
     * @brief initUi Called once to build the default RxOT UI
     */
    void duqf_initUi();
    /**
     * @brief duqf_setStyle Called once to set the UI Style after all ui have been created
     */
    void duqf_setStyle();
    /**
     * @brief Is the tool bar currently clicked or not
     */
    bool duqf_toolBarClicked;
    /**
     * @brief Drag position
     * Used for drag n drop feature
     */
    QPoint duqf_dragPosition;
    QToolButton *duqf_maximizeButton;
    QToolButton *duqf_settingsButton;
    QSettings settings;
    SettingsWidget *settingsWidget;
    QLabel *title;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MAINWINDOW_H
