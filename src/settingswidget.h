#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QFileDialog>
#include <QSettings>

#include "ui_settingswidget.h"

class SettingsWidget : public QWidget, private Ui::SettingsWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
private slots:
    void on_addIncludePathButton_clicked();
    void on_removeIncludePathButton_clicked();
    void on_includePathsList_indexesMoved(const QModelIndexList &indexes);
    void on_includePathsList_itemDoubleClicked(QListWidgetItem *item);

private:
    QSettings settings;
    void saveIncludePaths();
};

#endif // SETTINGSWIDGET_H
