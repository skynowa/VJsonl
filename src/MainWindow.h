/**
 * \file  MainWindow.h
 * \brief Declares the main application window.
 */


#pragma once

#include "JsonlModel.h"

#include <QMainWindow>
#include <QStringList>
//-------------------------------------------------------------------------------------------------
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
class QCloseEvent;
class QSplitter;
class JsonSyntaxHighlighter;
class LogFilterProxyModel;
//-------------------------------------------------------------------------------------------------
class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void openFile();
    void openFile(const QString &fileName);
    void onCurrentChanged(const QModelIndex &current);
    void updateCellView(const QModelIndex &current);
    void findInCellView();
    void addRecentFile(const QString &fileName);
    void loadRecentFiles();
    void updateRecentFilesMenu();
    void updateLogNameFilterItems();
    QString openDirectory() const;
    void saveOpenDirectory(const QString &fileName) const;
    void saveColumnWidths() const;
    void restoreColumnWidths();
    void savePanelLayout() const;
    void restorePanelLayout();
    void updateStatus();

private:
    JsonlModel            *_model {};
    LogFilterProxyModel   *_proxy {};
    QTableView            *_table {};
    QTextEdit             *_cellView {};
    QTextEdit             *_rawView {};
    JsonSyntaxHighlighter *_cellJsonHighlighter {};
    JsonSyntaxHighlighter *_rawJsonHighlighter {};
    QTextBrowser          *_htmlPreviewView {};
    QStackedWidget        *_cellStack {};
    QSplitter             *_mainSplitter {};
    QSplitter             *_detailsSplitter {};
    QCheckBox             *_format {};
    QCheckBox             *_wrapCellLine {};
    QCheckBox             *_htmlPreview {};
    QLineEdit             *_filter {};
    QLineEdit             *_cellSearch {};
    QComboBox             *_levelFilter {};
    QComboBox             *_logNameFilter {};
    QMenu                 *_recentFilesMenu {};
    QAction               *_openOriginalFileAction {};
    QAction               *_lightThemeAction {};
    QAction               *_greyThemeAction {};
    QAction               *_darkGreyThemeAction {};
    QAction               *_darkThemeAction {};
    QStringList            _recentFiles;
    QLabel                *_levelsStatusLabel {};
    QLabel                *_statusLabel {};
    QProgressBar          *_loadProgress {};
};
//-------------------------------------------------------------------------------------------------
