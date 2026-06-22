/**
 * \file  MainWindow.h
 * \brief Declares the main application window.
 */


#pragma once

#include "JsonlModel.h"
#include "TableSessionManager.h"

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
class QPoint;
class QAction;
class QActionGroup;
class QCloseEvent;
class QEvent;
class QSplitter;
class QWidget;
class JsonSyntaxHighlighter;
class LogFilterProxyModel;
//-------------------------------------------------------------------------------------------------
class MainWindow final :
    public QMainWindow
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
    void copyActiveCellValue();
    void copyFormattedCellValue();
    void copyRawCellValue();
    void copyFormattedRawCellValue();
    void findInTextView(QLineEdit *search, QTextEdit *view);
    void findInCellView();
    void findInRawView();
    void addRecentFile(const QString &fileName);
    void loadRecentFiles();
    void updateRecentFilesMenu();
    void updateColumnFilterItems(QComboBox *filter, const QString &columnName,
            const QString &allLabel);
    QString openDirectory() const;
    void saveOpenDirectory(const QString &fileName) const;
    void saveColumnWidths() const;
    void restoreColumnWidths();
    void saveColumnOrder() const;
    void restoreColumnOrder();
    void saveColumnVisibility() const;
    void restoreColumnVisibility();
    void showColumnVisibilityMenu(const QPoint &position);
    void loadTableSessions();
    void saveTableSessions();
    void rebuildTableSessionsMenu();
    void switchTableSession(const QString &name);
    void addTableSession();
    void renameTableSession();
    void removeTableSession();
    void applyActiveTableSession();
    void updateWindowTitle();
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
    QLineEdit             *_rawSearch {};
    QToolButton           *_copyValueButton {};
    QToolButton           *_copyFormattedValueButton {};
    QToolButton           *_copyRawValueButton {};
    QToolButton           *_copyFormattedRawValueButton {};
    QComboBox             *_projectFilter {};
    QComboBox             *_appFilter {};
    QComboBox             *_procNameFilter {};
    QComboBox             *_moduleFilter {};
    QComboBox             *_levelFilter {};
    QComboBox             *_logNameFilter {};
    QComboBox             *_descrFilter {};
    QLineEdit             *_queryFilter {};
    QLineEdit             *_requestFilter {};
    QLineEdit             *_pageFilter {};
    QMenu                 *_tsFilterMenu {};
    QToolButton           *_tsFilterButton {};
    QCheckBox             *_tsFromEnabled {};
    QCheckBox             *_tsToEnabled {};
    QDateTimeEdit         *_tsFrom {};
    QDateTimeEdit         *_tsTo {};
    QMenu                 *_recentFilesMenu {};
    QMenu                 *_tableSessionsMenu {};
    QActionGroup          *_tableSessionsActionGroup {};
    QAction               *_openOriginalFileAction {};
    QAction               *_lightThemeAction {};
    QAction               *_lightGrayThemeAction {};
    QAction               *_greyThemeAction {};
    QAction               *_mediumGrayThemeAction {};
    QAction               *_darkGreyThemeAction {};
    QAction               *_darkThemeAction {};
    QStringList            _recentFiles;
    QString                _activeCellValue;
    QString                _formattedCellValue;
    QString                _rawCellValue;
    QString                _formattedRawCellValue;
    QString                _loadedFileTitle;
    QLabel                *_levelsStatusLabel {};
    QLabel                *_statusLabel {};
    QProgressBar          *_loadProgress {};
    TableSessionManager    _tableSessions;
};
//-------------------------------------------------------------------------------------------------
