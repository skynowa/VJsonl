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
    // Construction
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    // Lifecycle and event handling
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    // File loading and recent files
    void openFile();
    void openFile(const QString &fileName);
    QString openDirectory() const;
    void saveOpenDirectory(const QString &fileName) const;
    void addRecentFile(const QString &fileName);
    void loadRecentFiles();
    void updateRecentFilesMenu();

    // Cell preview, raw preview, copy, and search actions
    void onCurrentChanged(const QModelIndex &current);
    void updateCellView(const QModelIndex &current);
    void copyActiveCellValue();
    void copyFormattedCellValue();
    void copyRawCellValue();
    void copyFormattedRawCellValue();
    void findInTextView(QLineEdit *search, QTextEdit *view);
    void findInCellView();
    void findInRawView();

    // Table filters and filter panel geometry
    void updateColumnFilterItems(QComboBox *filter, const QString &columnName,
            const QString &allLabel);
    void applyFilters();
    void updateTimestampFilterStatus();
    void updateFilterGeometry();
    void updateTimestampFilterBounds();

    // Column layout and visibility persistence
    void saveColumnWidths() const;
    void restoreColumnWidths();
    void saveColumnOrder() const;
    void restoreColumnOrder();
    void saveColumnVisibility() const;
    void restoreColumnVisibility();
    void showColumnVisibilityMenu(const QPoint &position);

    // Table sessions
    void loadTableSessions();
    void saveTableSessions();
    void rebuildTableSessionsMenu();
    void switchTableSession(const QString &name);
    void addTableSession();
    void renameTableSession();
    void removeTableSession();
    void applyActiveTableSession();

    // Window, panel layout, and status
    void updateWindowTitle();
    void savePanelLayout() const;
    void restorePanelLayout();
    void updateStatus();

private:
    // Data model and table view
    JsonlModel            *_model {};
    LogFilterProxyModel   *_proxy {};
    QTableView            *_table {};

    // Value preview panel and controls
    QTextEdit             *_cellView {};
    QTextEdit             *_rawView {};
    JsonSyntaxHighlighter *_cellJsonHighlighter {};
    JsonSyntaxHighlighter *_rawJsonHighlighter {};
    QTextBrowser          *_htmlPreviewView {};
    QStackedWidget        *_cellStack {};
    QCheckBox             *_format {};
    QCheckBox             *_wrapCellLine {};
    QCheckBox             *_htmlPreview {};
    QLineEdit             *_cellSearch {};
    QLineEdit             *_rawSearch {};
    QToolButton           *_copyValueButton {};
    QToolButton           *_copyFormattedValueButton {};
    QToolButton           *_copyRawValueButton {};
    QToolButton           *_copyFormattedRawValueButton {};

    // Panel containers
    QSplitter             *_mainSplitter {};
    QSplitter             *_detailsSplitter {};

    // Filter controls
    QWidget               *_filterPanel {};
    QLineEdit             *_filter {};
    QLineEdit             *_msgFilter {};
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

    // Menus and actions
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

    // Runtime state
    QStringList            _recentFiles;
    QString                _activeCellValue;
    QString                _formattedCellValue;
    QString                _rawCellValue;
    QString                _formattedRawCellValue;
    QString                _loadedFileTitle;

    // Status and persisted table sessions
    QLabel                *_levelsStatusLabel {};
    QLabel                *_statusLabel {};
    QProgressBar          *_loadProgress {};
    TableSessionManager    _tableSessions;
};
//-------------------------------------------------------------------------------------------------
