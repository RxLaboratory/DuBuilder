#ifndef INCLUDESETTINGSWIDGET_H
#define INCLUDESETTINGSWIDGET_H

#include <QFileDialog>
#include <QSettings>

#include "ui_includesettingswidget.h"

class IncludeSettingsWidget : public QWidget, private Ui::IncludeSettingsWidget
{
    Q_OBJECT

public:
    explicit IncludeSettingsWidget(QWidget *parent = nullptr);
private slots:
    void on_addIncludePathButton_clicked();
    void on_removeIncludePathButton_clicked();
    void on_includePathsList_indexesMoved(const QModelIndexList &indexes);
    void on_includePathsList_itemDoubleClicked(QListWidgetItem *item);

private:
    QSettings settings;
    void saveIncludePaths();
};

#endif // INCLUDESETTINGSWIDGET_H
