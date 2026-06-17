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
class QDateTimeEdit;
class QProgressBar;
class QToolButton;
class QStackedWidget;
class QTextBrowser;
class QMenu;
class QAction;
class QCloseEvent;
class QEvent;
class QSplitter;
class QWidget;
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
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void openFile();
    void openFile(const QString &fileName);
    void onCurrentChanged(const QModelIndex &current);
    void updateCellView(const QModelIndex &current);
    void findInCellView();
    void addRecentFile(const QString &fileName);
    void loadRecentFiles();
    void updateRecentFilesMenu();
    void updateColumnFilterItems(QComboBox *filter, const QString &columnName, const QString &allLabel);
    QString openDirectory() const;
    void saveOpenDirectory(const QString &fileName) const;
    void saveColumnWidths() const;
    void restoreColumnWidths();
    void savePanelLayout() const;
    void restorePanelLayout();
    void applyFilters();
    void updateTimestampFilterStatus();
    void updateFilterGeometry();
    void updateTimestampFilterBounds();
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
    QWidget               *_filterPanel {};
    QCheckBox             *_format {};
    QCheckBox             *_wrapCellLine {};
    QCheckBox             *_htmlPreview {};
    QLineEdit             *_filter {};
    QLineEdit             *_msgFilter {};
    QLineEdit             *_cellSearch {};
    QComboBox             *_projectFilter {};
    QComboBox             *_appFilter {};
    QComboBox             *_procNameFilter {};
    QComboBox             *_moduleFilter {};
    QComboBox             *_levelFilter {};
    QComboBox             *_logNameFilter {};
    QComboBox             *_queryFilter {};
    QMenu                 *_tsFilterMenu {};
    QToolButton           *_tsFilterButton {};
    QCheckBox             *_tsFromEnabled {};
    QCheckBox             *_tsToEnabled {};
    QDateTimeEdit         *_tsFrom {};
    QDateTimeEdit         *_tsTo {};
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
