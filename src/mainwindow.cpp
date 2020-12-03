#include "mainwindow.h"

#include <QtDebug>

MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent)
{

    // Build the form
    setupUi(this);
    // Add default stuff
    duqf_initUi();

    //SETTINGS

    IncludeSettingsWidget *i = new IncludeSettingsWidget();
    settingsWidget->addPage(i, "Include Paths", QIcon(":/icons/property"));

    //UI

    //populate toolbar

    //build options
    QMenu * buildMenu = new QMenu(this);
    //Comments options disabled for now as they're not working correctly, needs more testing
    /*buildMenu->addAction(actionIgnoreJSDoc);
    buildMenu->addAction(actionIgnoreBlockComments);
    buildMenu->addAction(actionIgnoreLineComments);
    buildMenu->addAction(actionKeepLicense);*/
    buildMenu->addAction(actionBuild_JSDoc);
    buildMenuButton = new QToolButton();
    buildMenuButton->setMenu(buildMenu);
    buildMenuButton->setPopupMode(QToolButton::InstantPopup);
    buildMenuButton->setText("Build Settings...");
    buildMenuButton->setIcon(QIcon(":/icons/build-settings"));
    buildMenuButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    mainToolBar->insertWidget(actionBuild, buildMenuButton);
    buildMenuButton->setEnabled(false);

    //add tree shortcuts
    treeWidget->installEventFilter(this);

    //initilization
    scanner = new Scanner();
    builder = new Builder();
    savePath = "";
    scanningItem = nullptr;
    currentScript = nullptr;

    //JSdoc build process
    jsdocProcess = new QProcess(this);

    mainStack->setCurrentIndex(0);

    //connexions
    mapEvents();

    //Process args
    outputFile = "";
    if (argc > 1)
    {
        //the first one is the input file
        QString inputFile(argv[1]);
        QString jsdocConfPath = "";
        //if there are only 2 args, the last one is the output file
        if (argc == 3) outputFile = argv[2];
        else if (argc > 3)
        {
            for (int i = 2; i < argc-1; i++)
            {
                QString arg(argv[i]);
                if ( (arg.toLower() == "-d" || arg.toLower() == "--jsdoc") && i < argc - 2) jsdocConfPath = argv[i+1];
            }
            //the last one is the output file
           outputFile = argv[argc-1];
        }

        if (jsdocConfPath != "")
        {
            QFileInfo jsdoc(jsdocConfPath);
            if (jsdoc.exists())
            {
                jsdocConfFile = jsdoc;
                actionBuild_JSDoc->setChecked(true);
                qInfo().noquote() << "Building JSDoc from: " + jsdocConfPath;
            }
            else
            {
                qInfo().noquote() << "Sorry, cannot read the jsdoc conf file: " + jsdocConfPath;
                qInfo() << "JSdoc will not be built";
            }
        }

        if (inputFile != "")
        {
            bool ok = openFile(inputFile);
            if (ok)
            {
                qInfo().noquote() << "Opening: " + inputFile;
            }
            else
            {
                qInfo().noquote() << "Sorry, Cannot open file: " + inputFile;
            }
        }

    }

    // Set style
    duqf_setStyle();
}

void MainWindow::mapEvents()
{
    connect(scanner,SIGNAL(finished(Script*)),this,SLOT(scanned(Script*)));
    connect(scanner,SIGNAL(started()),this,SLOT(setWaiting()));
    connect(scanner,SIGNAL(progress(int, QString)),this,SLOT(progress(int, QString)));
    connect(builder,SIGNAL(built(QString)),this,SLOT(built(QString)));
    connect(builder,SIGNAL(started()),this,SLOT(setWaiting()));
    connect(builder,SIGNAL(progress(int,QString)), this, SLOT(progress(int, QString)));
    connect(jsdocProcess,SIGNAL(finished(int, QProcess::ExitStatus)),this, SLOT(jsdocBuilt(int, QProcess::ExitStatus)));
    connect(jsdocProcess,SIGNAL(readyReadStandardError()),this, SLOT(jsdocOutput()));
    connect(jsdocProcess,SIGNAL(readyReadStandardOutput()),this, SLOT(jsdocOutput()));
    connect(jsdocProcess,SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(jsdocError(QProcess::ProcessError)));
}

bool MainWindow::openFile(QString filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) return false;
    settings.setValue("latestopenpath",fileInfo.path());
    progressBar->setMaximum(1);

    //scan
    scanningItem = nullptr;
    scanner->setFile(filePath);
    scanner->setRecursive(true);
    scanner->start();
    return true;
}

bool MainWindow::buildFile(QString filePath)
{
    savePath = filePath;
    QFileInfo fileInfo(savePath);
    settings.setValue("latestsavepath",fileInfo.path());

    //build
    builder->setScript(currentScript);
    builder->setIgnoreJSDoc(actionIgnoreJSDoc->isChecked());
    builder->setIgnoreLineComments(actionIgnoreLineComments->isChecked());
    builder->setIgnoreBlockComments(actionIgnoreBlockComments->isChecked());
    builder->setKeepLicense(actionKeepLicense->isChecked());
    builder->start();
    return true;
}

// ACTIONS

void MainWindow::on_actionOpen_Script_triggered()
{
    //open file
    QString scriptPath = QFileDialog::getOpenFileName(this,"Select script",settings.value("latestopenpath").toString(),"All scripts (*.jsx *.jsxinc *.js);;ExtendScript (*.jsx *.jsxinc);;JavaScript (*.js);;Text (*.txt);;All Files (*.*)");
    if (scriptPath.isNull() || scriptPath.isEmpty()) return;
    openFile(scriptPath);
}

void MainWindow::on_actionRe_scan_script_triggered()
{
    //not ready!
    actionRe_scan_script->setEnabled(false);
    actionBuild->setEnabled(false);
    actionCollect_Files->setEnabled(false);
    title->setText("DuBuilder");
    this->setWindowTitle("DuBuilder");
    this->repaint();
    progressBar->setMaximum(1);

    scanningItem = nullptr;
    scanner->setFile(currentScript->file()->fileName());
    scanner->setRecursive(true);
    scanner->start();
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    //open file
    QString newPath = QFileDialog::getOpenFileName(this,"Where is " + item->text(2) + "?",settings.value("latestincludepath").toString());

    if (newPath.isNull() || newPath.isEmpty()) return;
    settings.setValue("latestincludepath",QFileInfo(newPath).path());

    //scan file
    scanningItem = item;
    scanner->setFile(newPath);
    scanner->setRecursive(true);
    scanner->start();

}

void MainWindow::on_actionBuild_triggered()
{
    QString path = QFileDialog::getSaveFileName(this,"Select script",settings.value("latestsavepath").toString() + currentScript->completeName(),"All scripts (*.jsx *.jsxinc *.js);;ExtendScript (*.jsx *.jsxinc);;JavaScript (*.js);;Text (*.txt);;All Files (*.*)");
    if (path.isNull() || path.isEmpty()) return;
    buildFile(path);
}

void MainWindow::on_actionBuild_JSDoc_triggered(bool checked)
{
    if (!checked) return;

    QString file = QFileDialog::getOpenFileName(this,"Select the jsdoc conf file","","*.json");
    if (file.isNull() || file.isEmpty())
    {
        actionBuild_JSDoc->setChecked(false);
        return;
    }
    jsdocConfFile = QFileInfo(file);
}

void MainWindow::removeCurrentIncludeItems()
{
    QList<QTreeWidgetItem*> selectedItems = treeWidget->selectedItems();
    for (int i = 0; i < selectedItems.count(); i++)
    {
        QTreeWidgetItem *item = selectedItems[i];

        removeInclude(currentScript, item->data(0,Qt::UserRole).toInt());

        item->setText(3, "Removed");
        item->setIcon(0,QIcon(":/icons/warning"));
    }
}

// OTHER SLOTS

void MainWindow::scanned(Script *script)
{
#ifdef QT_DEBUG
    qDebug() << "Got new script ===== " + script->name();
#endif

    if (scanningItem == nullptr && script != nullptr)
    {
        delete currentScript;
        currentScript = script;
        currentScript->setParent(this);
        //empty tree
        treeWidget->clear();

        //main script
        this->setWindowTitle(script->name());

        foreach(Script *s,script->includes())
        {
            treeWidget->addTopLevelItem(createIncludeItem(s));
        }

        //ready!
        actionRe_scan_script->setEnabled(true);
        actionBuild->setEnabled(true);
        actionCollect_Files->setEnabled(true);
        buildMenuButton->setEnabled(true);
        title->setText(currentScript->completeName());
    }
    else
    {
        if (script == nullptr)
        {
            //update display
            scanningItem->setText(3,"Removed");
            scanningItem->setIcon(0,QIcon(":/icons/warning"));
        }
        else
        {
            //update display of item
            scanningItem->setText(3,script->file()->fileName());
            scanningItem->setIcon(0,QIcon(":/icons/ok"));
            //remove old childs
            QList<QTreeWidgetItem *> items = scanningItem->takeChildren();
            while(items.count() > 0)
            {
                QTreeWidgetItem *item = items.takeAt(0);
                delete item;
            }
            //update includes list of currentScript
            script->setId(scanningItem->data(0,Qt::UserRole).toInt());
            script->setLine(scanningItem->text(1).toInt());

            updateScript(currentScript,script);

            foreach(Script *s,script->includes())
            {
                scanningItem->addChild(createIncludeItem(s));
            }
        }
    }

    setWaiting(false);

    //If there's an output file, let's build this!
    if (outputFile != "")
    {
        bool ok = buildFile(outputFile);
        if (ok)
        {
            qInfo().noquote() << "Building to: " + outputFile;
        }
        else
        {
            qInfo().noquote() << "Sorry, Cannot build to: " + outputFile;
            qApp->quit();
        }
    }
}

void MainWindow::built(QString builtScript)
{
    if (builtScript == "")
    {
        //TODO Display Error
        return;
    }

    QFile saveFile(savePath);
    if ( !saveFile.open(QIODevice::WriteOnly | QIODevice::Text) )
    {
       //TODO Display Error
        return;
    }
    saveFile.write(builtScript.toUtf8());
    saveFile.close();
    builtScript = "";

    if (actionBuild_JSDoc->isChecked() && jsdocConfFile.exists())
    {
         QString cmd = "cmd.exe /c \"jsdoc -c \"" + jsdocConfFile.fileName() + "\"\"";
         jsdocProcess->setWorkingDirectory(jsdocConfFile.dir().absolutePath());
         jsdocProcess->start(cmd);
         progress(progressBar->maximum(), "Building jsdoc...");
         //If the output file was set by command line, print info and continue
         if( outputFile != "")
         {
             qInfo() << "Building JSDoc";
             return;
         }
    }
    else
    {
        setWaiting(false);
    }

    //If the output file was set by command line, print info and quit
    if (outputFile != "")
    {
        qInfo() << "Build finished.";
        qApp->quit();
    }
}

void MainWindow::jsdocBuilt(int exitCode, QProcess::ExitStatus exitStatus)
{
    //TODO Check exit code and status
    setWaiting(false);

    //If the output file was set by command line, print info and quit
    if (outputFile != "")
    {
        qInfo() << "Build finished.";
        qApp->quit();
    }
}

void MainWindow::jsdocOutput()
{
    qInfo().noquote() << jsdocProcess->readAll();
}

void MainWindow::jsdocError(QProcess::ProcessError error)
{
    qDebug() << error;
    qWarning().noquote() << jsdocProcess->errorString();
    setWaiting(false);
}

void MainWindow::progress(int i, QString message)
{
    progressBar->setValue(i);
    if (i < 0)
    {
        progressBar->setValue(0);
        progressBar->setFormat(message);
    }
    else
    {
        progressBar->setFormat("%p% | " + message);
    }
}

// METHODS

QTreeWidgetItem *MainWindow::createIncludeItem(Script *script)
{
    QStringList attributes;

    //path
    QFile *scriptFile = script->file();
    QString path = QFileInfo(*scriptFile).absoluteFilePath();

    attributes << "" << QString::number(script->line()) << script->name() << path;

    QTreeWidgetItem *scriptItem = new QTreeWidgetItem(attributes);
    if (script->exists()) scriptItem->setIcon(0,QIcon(":/icons/ok"));
    else scriptItem->setIcon(0,QIcon(":/icons/warning"));
    scriptItem->setData(0,Qt::UserRole,script->id());
    scriptItem->setExpanded(true);

    progressBar->setMaximum(progressBar->maximum() + 1);

    //add children
    foreach(Script *s,script->includes())
    {
        scriptItem->addChild(createIncludeItem(s));
    }

    return scriptItem;
}

bool MainWindow::updateScript(Script *containingScript, Script *newScript)
{

    for( int i = 0 ; i <  containingScript->includes().count() ; i++)
    {
        Script *s = containingScript->includes().at(i);
        if (s == nullptr) continue;
        if (s->id() == newScript->id())
        {
            Script *oldScript = containingScript->takeInclude(i);
            delete oldScript;
            containingScript->addInclude(newScript);
            return true;
        }
        else
        {
            if (updateScript(s,newScript)) return true;
        }
    }
    return false;
}

void MainWindow::removeInclude(Script *containingScript, int scriptId)
{
    qDebug() << "removing script " + QString::number(scriptId);
    for( int i = containingScript->includes().count()-1 ; i >= 0 ; i--)
    {
        Script *s = containingScript->includes().at(i);
        if (s == nullptr) continue;
        if (s->id() == scriptId)
        {
            Script *include = containingScript->takeInclude(i);
            qDebug() << "removed script " + include->name();
            delete include;
        }
        else
        {
            removeInclude(s, scriptId);
        }
    }
}

void MainWindow::removeInclude(Script *containingScript, Script *includeScript)
{
    removeInclude(containingScript, includeScript->id());
}

// UI

void MainWindow::setWaiting(bool wait)
{

    if (wait)
    {
        setCursor(Qt::BusyCursor);
        mainToolBar->setEnabled(false);
        mainStack->setCurrentIndex(2);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        mainToolBar->setEnabled(true);
        mainStack->setCurrentIndex(0);
    }
    repaint();
}

// DuQF


void MainWindow::duqf_initUi()
{
    this->setWindowIcon(QIcon(APP_ICON));
    // ===== TOOLBAR ======

    // remove right click on toolbar
    mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    // set frameless
#ifndef Q_OS_MAC
    this->setWindowFlags(Qt::FramelessWindowHint);
#endif
    //drag window
    duqf_toolBarClicked = false;
    mainToolBar->installEventFilter(this);

    // ==== TOOLBAR BUTTONS
    mainToolBar->addWidget(new ToolBarSpacer());
    title = new QLabel(STR_FILEDESCRIPTION);
    mainToolBar->addWidget(title);
    //minimize
#ifndef Q_OS_MAC
    QToolButton *minimizeButton = new QToolButton();
    minimizeButton->setIcon(QIcon(":/icons/minimize"));
    minimizeButton->setObjectName("windowButton");
    mainToolBar->addWidget(minimizeButton);
#endif
    //maximize
    duqf_maximizeButton = new QToolButton(this);
    duqf_maximizeButton->setIcon(QIcon(":/icons/maximize"));
    duqf_maximizeButton->setObjectName("windowButton");
    mainToolBar->addWidget(duqf_maximizeButton);
    //quit
    QToolButton *quitButton = new QToolButton(this);
    quitButton->setIcon(QIcon(":/icons/quit"));
    quitButton->setObjectName("windowButton");
    mainToolBar->addWidget(quitButton);

    // ===== STATUSBAR ======

    // version in statusbar
    mainStatusBar->addPermanentWidget(new QLabel("v" + QString(STR_VERSION)));
    duqf_settingsButton = new QToolButton();
    duqf_settingsButton->setIcon(QIcon(":/icons/settings"));
    duqf_settingsButton->setToolTip("Go to Settings");
    duqf_settingsButton->setCheckable(true);
    mainStatusBar->addPermanentWidget(duqf_settingsButton);
    QToolButton *helpButton = new QToolButton();
    helpButton->setIcon(QIcon(":/icons/help"));
    helpButton->setToolTip("Get Help");
    helpButton->setPopupMode( QToolButton::InstantPopup );
    QMenu *helpMenu = new QMenu(this);
    if (QString(URL_DOC) != "")
    {
        QAction *docAction = new QAction(QIcon(":/icons/documentation"), "Help");
        docAction->setToolTip("Read the documentation");
        docAction->setShortcut(QKeySequence("F1"));
        helpMenu->addAction(docAction);
        helpMenu->addSeparator();
        connect(docAction, SIGNAL(triggered()), this, SLOT(duqf_doc()));
    }
    bool chat = QString(URL_CHAT) != "";
    bool bugReport = QString(URL_BUGREPORT) != "";
    bool forum = QString(URL_FORUM) != "";
    if (bugReport)
    {
        QAction *bugReportAction = new QAction(QIcon(":/icons/bug-report"), "Bug Report");
        bugReportAction->setToolTip("Report a bug");
        helpMenu->addAction(bugReportAction);
        if (!chat && !forum) helpMenu->addSeparator();
        connect(bugReportAction, SIGNAL(triggered()), this, SLOT(duqf_bugReport()));
    }
    if (chat)
    {
        QAction *chatAction = new QAction(QIcon(":/icons/chat"), "Chat");
        chatAction->setToolTip("Come and have a chat");
        helpMenu->addAction(chatAction);
        if (!forum) helpMenu->addSeparator();
        connect(chatAction, SIGNAL(triggered()), this, SLOT(duqf_chat()));
    }
    if (forum)
    {
        QAction *forumAction = new QAction(QIcon(":/icons/forum"), "Forum");
        forumAction->setToolTip("Join us on our forum");
        helpMenu->addAction(forumAction);
        helpMenu->addSeparator();
        connect(forumAction, SIGNAL(triggered()), this, SLOT(duqf_forum()));
    }
    QAction *aboutQtAction = new QAction(QIcon(":/icons/qt"), "About Qt");
    helpMenu->addAction(aboutQtAction);

    helpButton->setMenu(helpMenu);
    mainStatusBar->addPermanentWidget(helpButton);

    // ========= SETTINGS ========

    settingsWidget = new SettingsWidget();
    duqf_settingsLayout->addWidget(settingsWidget);
    duqf_closeSettingsButton->setObjectName("windowButton");

    AppearanceSettingsWidget *asw = new AppearanceSettingsWidget();
    settingsWidget->addPage(asw, "Appearance", QIcon(":/icons/color"));

    // ====== CONNECTIONS ======
    connect(duqf_maximizeButton,SIGNAL(clicked()),this,SLOT(duqf_maximize()));
    connect(minimizeButton,SIGNAL(clicked()),this,SLOT(showMinimized()));
    connect(quitButton,SIGNAL(clicked()),this,SLOT(close()));

    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(duqf_settingsButton, SIGNAL(clicked(bool)), this, SLOT(duqf_settings(bool)));
    connect(duqf_closeSettingsButton, SIGNAL(clicked()), this, SLOT(duqf_closeSettings()));
}

void MainWindow::duqf_setStyle()
{
    // ======== STYLE ========

    //Re-set StyleSheet
    QString cssFile = settings.value("appearance/cssFile", ":/styles/default").toString();
    QString style = settings.value("appearance/style","Default").toString();
    if (cssFile != "")
    {
        DuUI::updateCSS(cssFile);
    }
    else
    {
        DuUI::updateCSS("");
        qApp->setStyle(QStyleFactory::create(style));
    }
    //and font
    DuUI::setFont(settings.value("appearance/font", "Ubuntu").toString());
    //and tool buttons
    int styleIndex = settings.value("appearance/toolButtonStyle", 2).toInt();
    Qt::ToolButtonStyle toolStyle = Qt::ToolButtonTextUnderIcon;
    if (styleIndex == 0) toolStyle = Qt::ToolButtonIconOnly;
    else if (styleIndex == 1) toolStyle = Qt::ToolButtonTextOnly;
    else if (styleIndex == 2) toolStyle = Qt::ToolButtonTextUnderIcon;
    else if (styleIndex == 3) toolStyle = Qt::ToolButtonTextBesideIcon;
    DuUI::setToolButtonStyle(toolStyle);
}

void MainWindow::duqf_maximize(bool max)
{
    if (!max)
    {
        duqf_maximizeButton->setIcon(QIcon(":/icons/maximize"));
        this->showNormal();
    }
    else
    {
        duqf_maximizeButton->setIcon(QIcon(":/icons/unmaximize"));
        this->showMaximized();
    }
}

void MainWindow::duqf_maximize()
{
    duqf_maximize(!this->isMaximized());
}

void MainWindow::duqf_bugReport()
{
    QDesktopServices::openUrl ( QUrl( URL_BUGREPORT ) );
}

void MainWindow::duqf_forum()
{
    QDesktopServices::openUrl ( QUrl( URL_FORUM ) );
}

void MainWindow::duqf_chat()
{
    QDesktopServices::openUrl ( QUrl( URL_CHAT ) );
}

void MainWindow::duqf_doc()
{
    QDesktopServices::openUrl ( QUrl( URL_DOC ) );
}

void MainWindow::duqf_settings(bool checked)
{
    duqf_settingsButton->setChecked(checked);
    if (checked)
    {
        mainStack->setCurrentIndex(1);
    }
    else
    {
        mainStack->setCurrentIndex(0);
    }
}

void MainWindow::duqf_closeSettings()
{
    duqf_settings(false);
}

// EVENTS

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = (QMouseEvent*)event;
        if (mouseEvent->button() == Qt::LeftButton)
        {
            duqf_toolBarClicked = true;
            duqf_dragPosition = mouseEvent->globalPos() - this->frameGeometry().topLeft();
            event->accept();
        }
        return true;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        if (this->isMaximized()) return false;
        QMouseEvent *mouseEvent = (QMouseEvent*)event;
        if (mouseEvent->buttons() & Qt::LeftButton && duqf_toolBarClicked)
        {
            this->move(mouseEvent->globalPos() - duqf_dragPosition);
            event->accept();
        }
        return true;
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        duqf_toolBarClicked = false;
        return true;
    }
#ifndef Q_OS_MAC
    else if (event->type() == QEvent::MouseButtonDblClick)
    {
        duqf_maximize();
        event->accept();
        return true;
    }
#endif
  else if ( event->type() == QEvent::KeyPress )
  {
      QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
      if( keyEvent->key() == Qt::Key_Delete ){
          removeCurrentIncludeItems();
          return true;
      }
  }

  // standard event processing
  return QObject::eventFilter(obj, event);
}

