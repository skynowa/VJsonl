#pragma once

#include "JsonlModel.h"

#include <QMainWindow>
#include <QStringList>

class QLabel;
class QLineEdit;
class QModelIndex;
class QTableView;
class QTextEdit;
class QCheckBox;
class QComboBox;
class QProgressBar;
class QStackedWidget;
class QTextBrowser;
class QMenu;
class QAction;
class LogFilterProxyModel;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void openFile();
    void openFile(const QString &fileName);
    void onCurrentChanged(const QModelIndex &current);
    void updateCellView(const QModelIndex &current);
    void findInCellView();
    void addRecentFile(const QString &fileName);
    void loadRecentFiles();
    void updateRecentFilesMenu();
    void updateStatus();

private:
    JsonlModel            *_model {};
    LogFilterProxyModel   *_proxy {};
    QTableView            *_table {};
    QTextEdit             *_cellView {};
    QTextBrowser          *_htmlPreviewView {};
    QStackedWidget        *_cellStack {};
    QCheckBox             *_format {};
    QCheckBox             *_wrapCellLine {};
    QCheckBox             *_htmlPreview {};
    QLineEdit             *_filter {};
    QLineEdit             *_cellSearch {};
    QComboBox             *_levelFilter {};
    QMenu                 *_recentFilesMenu {};
    QAction               *_openOriginalFileAction {};
    QStringList            _recentFiles;
    QLabel                *_statusLabel {};
    QProgressBar          *_loadProgress {};
};
