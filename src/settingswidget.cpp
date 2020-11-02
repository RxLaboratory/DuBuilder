#include "settingswidget.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    //populate UI
    int numIncludePaths = settings.beginReadArray("includePaths");
    for (int i = 0; i < numIncludePaths; i++)
    {
        settings.setArrayIndex(i);
        includePathsList->addItem(settings.value("path").toString());
    }
    settings.endArray();
}

void SettingsWidget::on_addIncludePathButton_clicked()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Select the new include path.");
    if (folder == "") return;
    QListWidgetItem *item = new QListWidgetItem(folder);
    includePathsList->addItem(item);
    saveIncludePaths();
}

void SettingsWidget::on_removeIncludePathButton_clicked()
{
    qDeleteAll(includePathsList->selectedItems());
    saveIncludePaths();
}

void SettingsWidget::on_includePathsList_indexesMoved(const QModelIndexList &indexes)
{
    saveIncludePaths();
}

void SettingsWidget::on_includePathsList_itemDoubleClicked(QListWidgetItem *item)
{
    QString folder = QFileDialog::getExistingDirectory(this, "Select the include path.");
    if (folder == "") return;
    item->setText(folder);
    saveIncludePaths();
}

void SettingsWidget::saveIncludePaths()
{
    settings.remove("includePaths");
    settings.beginWriteArray("includePaths");
    for (int i = 0; i < includePathsList->count(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("path",includePathsList->item(i)->text());
    }
    settings.endArray();
}
