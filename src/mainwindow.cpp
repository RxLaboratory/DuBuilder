#include "mainwindow.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    //UI

    //remove right click on toolbar
    mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    //populate toolbar

    //build options
    QMenu * buildMenu = new QMenu(this);
    buildMenu->addAction(actionIgnoreJSDoc);
    buildMenu->addAction(actionIgnoreBlockComments);
    buildMenu->addAction(actionIgnoreLineComments);
    buildMenu->addAction(actionKeepLicense);
    buildMenuButton = new QToolButton();
    buildMenuButton->setMenu(buildMenu);
    buildMenuButton->setPopupMode(QToolButton::InstantPopup);
    buildMenuButton->setText("Build Settings...");
    buildMenuButton->setIcon(QIcon(":/icons/file-settings"));
    buildMenuButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mainToolBar->insertWidget(actionBuild, buildMenuButton);
    buildMenuButton->setEnabled(false);

    ToolBarSpacer *tbs = new ToolBarSpacer();
    mainToolBar->addWidget(tbs);

    //title
    titleLabel = new QLabel("");
    mainToolBar->addWidget(titleLabel);

    //window buttons
#ifndef Q_OS_MAC
    // Maximize and minimize only on linux and windows
    this->setWindowFlags(Qt::FramelessWindowHint);
    maximizeButton = new QPushButton(QIcon(":/icons/maximize"),"");
    minimizeButton = new QPushButton(QIcon(":/icons/minimize"),"");
    mainToolBar->addWidget(minimizeButton);
    mainToolBar->addWidget(maximizeButton);
#endif
    quitButton = new QPushButton(QIcon(":/icons/close"),"");
    mainToolBar->addWidget(quitButton);

    //drag window
    toolBarClicked = false;
    mainToolBar->installEventFilter(this);

    //set style
    QString css = RainboxUI::loadCSS(":/styles/default");
    qApp->setStyleSheet(css);

    //add tree shortcuts
    treeWidget->installEventFilter(this);

    //settings widget
    settingsWidget = new SettingsWidget(this);
    settingsPage->layout()->addWidget(settingsWidget);

    //initilization
    scanner = new Scanner();
    builder = new Builder();
    savePath = "";
    scanningItem = nullptr;
    currentScript = nullptr;

    mainStack->setCurrentIndex(0);

    //connexions
    mapEvents();
}

void MainWindow::mapEvents()
{
    connect(scanner,SIGNAL(finished(Script*)),this,SLOT(scanned(Script*)));
    connect(scanner,SIGNAL(started()),this,SLOT(setWaiting()));
    connect(scanner,SIGNAL(progress(int, QString)),this,SLOT(progress(int, QString)));
    connect(builder,SIGNAL(built(QString)),this,SLOT(built(QString)));
    connect(builder,SIGNAL(started()),this,SLOT(setWaiting()));
    connect(builder,SIGNAL(progress(int,QString)), this, SLOT(progress(int, QString)));

    // Window management
#ifndef Q_OS_MAC
    // Windows and linux
    connect(maximizeButton,SIGNAL(clicked()),this,SLOT(maximize()));
    connect(minimizeButton,SIGNAL(clicked()),this,SLOT(showMinimized()));
#endif
    connect(quitButton,SIGNAL(clicked()),qApp,SLOT(quit()));
}

// ACTIONS

void MainWindow::on_actionOpen_Script_triggered()
{
    //open file
    QString scriptPath = QFileDialog::getOpenFileName(this,"Select script",settings.value("latestopenpath").toString(),"All scripts (*.jsx *.jsxinc *.js);;ExtendScript (*.jsx *.jsxinc);;JavaScript (*.js);;Text (*.txt);;All Files (*.*)");
    if (scriptPath.isNull() || scriptPath.isEmpty()) return;
    settings.setValue("latestopenpath",QFileInfo(scriptPath).path());
    progressBar->setMaximum(1);

    //scan
    scanningItem = nullptr;
    scanner->setFile(scriptPath);
    scanner->setRecursive(true);
    scanner->start();
}

void MainWindow::on_actionRe_scan_script_triggered()
{
    //not ready!
    actionRe_scan_script->setEnabled(false);
    actionBuild->setEnabled(false);
    actionCollect_Files->setEnabled(false);
    titleLabel->setText("");
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
    savePath = QFileDialog::getSaveFileName(this,"Select script",settings.value("latestsavepath").toString(),"All scripts (*.jsx *.jsxinc *.js);;ExtendScript (*.jsx *.jsxinc);;JavaScript (*.js);;Text (*.txt);;All Files (*.*)");
    if (savePath.isNull() || savePath.isEmpty()) return;
    settings.setValue("latestsavepath",QFileInfo(savePath).path());

    //build
    builder->setScript(currentScript);
    builder->setIgnoreJSDoc(actionIgnoreJSDoc->isChecked());
    builder->setIgnoreLineComments(actionIgnoreLineComments->isChecked());
    builder->setIgnoreBlockComments(actionIgnoreBlockComments->isChecked());
    builder->setKeepLicense(actionKeepLicense->isChecked());
    builder->start();
}

void MainWindow::on_actionSettings_triggered(bool checked)
{
    if (checked) mainStack->setCurrentIndex(1);
    else mainStack->setCurrentIndex(0);
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
        titleLabel->setText(currentScript->completeName());
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

#ifndef Q_OS_MAC
void MainWindow::maximize()
{

    if (this->isMaximized())
    {
        maximizeButton->setIcon(QIcon(":/icons/maximize"));
        this->showNormal();
    }
    else
    {
        maximizeButton->setIcon(QIcon(":/icons/unmaximize"));
        this->showMaximized();
    }

}
#endif

// EVENTS

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::MouseButtonPress)
  {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
      if (mouseEvent->button() == Qt::LeftButton)
      {
        toolBarClicked = true;
        dragPosition = mouseEvent->globalPos() - this->frameGeometry().topLeft();
        event->accept();
      }
      return true;
  }
  else if (event->type() == QEvent::MouseMove)
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    if (mouseEvent->buttons() & Qt::LeftButton && toolBarClicked)
    {
        if (this->isMaximized()) this->showNormal();
        this->move(mouseEvent->globalPos() - dragPosition);
        event->accept();
    }
    return true;
  }
  else if (event->type() == QEvent::MouseButtonRelease)
  {
      toolBarClicked = false;
      return true;
  }
#ifndef Q_OS_MAC
  else if (event->type() == QEvent::MouseButtonDblClick)
  {
      maximize();
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

