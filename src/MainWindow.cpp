/**
 * \file  MainWindow.cpp
 * \brief Implements the main Qt Widgets window and user interactions.
 */


#include "MainWindow.h"

#include "AppSettings.h"
#include "BuildInfo.h"
#include "CodeFormatter.h"
#include "Delegates.h"
#include "Utils/Demangle.h"
#include "Utils/File.h"
#include "Utils/Filter.h"
#include "Utils/Html.h"
#include "Utils/Icon.h"
#include "JsonSyntaxHighlighter.h"
#include "LogFilterProxyModel.h"
#include "LogLevelStyle.h"
#include "ThemeManager.h"
#include "Utils/TextSearch.h"
#include "Utils/Timestamp.h"

#include <QAbstractItemView>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QClipboard>
#include <QComboBox>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QDesktopServices>
#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QSettings>
#include <QShortcut>
#include <QSizePolicy>
#include <QScrollBar>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QSignalBlocker>
#include <QTableView>
#include <QTextBrowser>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QWidget>
#include <QUrl>

//-------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    _model = new JsonlModel(this);

    _proxy = new LogFilterProxyModel(this);
    _proxy->setSourceModel(_model);
    _proxy->setDynamicSortFilter(true);

    _filter = new QLineEdit(this);
    _filter->setPlaceholderText(QStringLiteral("Filter text in all columns..."));
    _filter->setClearButtonEnabled(true);
    _filter->setMinimumWidth(0);

    _msgFilter = new QLineEdit(this);
    _msgFilter->setPlaceholderText(QStringLiteral("Filter msg..."));
    _msgFilter->setClearButtonEnabled(true);
    _msgFilter->setMinimumWidth(0);

    _projectFilter = new QComboBox(this);
    _projectFilter->addItem(QStringLiteral("All projects"), QString());
    _projectFilter->setMinimumWidth(0);

    _appFilter = new QComboBox(this);
    _appFilter->addItem(QStringLiteral("All apps"), QString());
    _appFilter->setMinimumWidth(0);

    _procNameFilter = new QComboBox(this);
    _procNameFilter->addItem(QStringLiteral("All proc names"), QString());
    _procNameFilter->setMinimumWidth(0);

    _moduleFilter = new QComboBox(this);
    _moduleFilter->addItem(QStringLiteral("All modules"), QString());
    _moduleFilter->setMinimumWidth(0);

    _levelFilter = new QComboBox(this);
    _levelFilter->addItem(QStringLiteral("All levels"), QString());
    _levelFilter->setMinimumWidth(0);
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("fatal")), QStringLiteral("Fatal"), QStringLiteral("fatal"));
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("error")), QStringLiteral("Error"), QStringLiteral("error"));
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("warn")), QStringLiteral("Warn"), QStringLiteral("warn"));
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("warning")), QStringLiteral("Warning"), QStringLiteral("warning"));
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("debug")), QStringLiteral("Debug"), QStringLiteral("debug"));
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("info")), QStringLiteral("Info"), QStringLiteral("info"));
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("trace")), QStringLiteral("Trace"), QStringLiteral("trace"));

    _logNameFilter = new QComboBox(this);
    _logNameFilter->addItem(QStringLiteral("All log names"), QString());
    _logNameFilter->setMinimumWidth(0);

    _queryFilter = new QLineEdit(this);
    _queryFilter->setPlaceholderText(QStringLiteral("Filter query..."));
    _queryFilter->setClearButtonEnabled(true);
    _queryFilter->setMinimumWidth(0);

    _tsFilterButton = new QToolButton(this);
    _tsFilterButton->setText(QStringLiteral("off"));
    _tsFilterButton->setIcon(icon_utils::calendarIcon());
    _tsFilterButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _tsFilterButton->setPopupMode(QToolButton::InstantPopup);
    _tsFilterButton->setMinimumWidth(0);

    _tsFilterMenu = new QMenu(_tsFilterButton);
    auto *tsFilterWidget = new QWidget(_tsFilterMenu);
    auto *tsFilterLayout = new QHBoxLayout(tsFilterWidget);
    tsFilterLayout->setContentsMargins(0, 0, 0, 0);
    tsFilterLayout->setSpacing(2);

    _tsFromEnabled = new QCheckBox(QStringLiteral("from"), tsFilterWidget);
    _tsFrom = new QDateTimeEdit(QDateTime::currentDateTime(), tsFilterWidget);
    _tsFrom->setCalendarPopup(true);
    _tsFrom->setDisplayFormat(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
    _tsFrom->setEnabled(false);

    _tsToEnabled = new QCheckBox(QStringLiteral("to"), tsFilterWidget);
    _tsTo = new QDateTimeEdit(QDateTime::currentDateTime(), tsFilterWidget);
    _tsTo->setCalendarPopup(true);
    _tsTo->setDisplayFormat(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
    _tsTo->setEnabled(false);

    tsFilterLayout->addWidget(_tsFromEnabled);
    tsFilterLayout->addWidget(_tsFrom, 1);
    tsFilterLayout->addWidget(_tsToEnabled);
    tsFilterLayout->addWidget(_tsTo, 1);
    auto *tsFilterAction = new QWidgetAction(_tsFilterMenu);
    tsFilterAction->setDefaultWidget(tsFilterWidget);
    _tsFilterMenu->addAction(tsFilterAction);
    _tsFilterButton->setMenu(_tsFilterMenu);

    _table = new QTableView(this);
    _table->setModel(_proxy);
    _table->setSortingEnabled(true);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->setSelectionMode(QAbstractItemView::SingleSelection);
    _table->setAlternatingRowColors(true);
    _table->setItemDelegate(new ActiveCellDelegate(_table, _table));
    _table->horizontalHeader()->setStretchLastSection(false);
    QFont headerFont = _table->horizontalHeader()->font();
    headerFont.setBold(true);
    _table->horizontalHeader()->setFont(headerFont);
    _table->verticalHeader()->setFont(headerFont);
    _table->viewport()->installEventFilter(this);
    _table->horizontalHeader()->installEventFilter(this);

    _cellView = new QTextEdit(this);
    _cellView->setReadOnly(true);
    _cellView->setLineWrapMode(QTextEdit::NoWrap);
    _cellView->setFontFamily(QStringLiteral("monospace"));
    _cellJsonHighlighter = new JsonSyntaxHighlighter(_cellView->document());

    _rawView = new QTextEdit(this);
    _rawView->setReadOnly(true);
    _rawView->setLineWrapMode(QTextEdit::NoWrap);
    _rawView->setFontFamily(QStringLiteral("monospace"));
    _rawJsonHighlighter = new JsonSyntaxHighlighter(_rawView->document());

    _htmlPreviewView = new QTextBrowser(this);
    _htmlPreviewView->setOpenExternalLinks(false);

    _cellStack = new QStackedWidget(this);
    _cellStack->addWidget(_cellView);
    _cellStack->addWidget(_htmlPreviewView);

    _format = new QCheckBox(QStringLiteral("Format"), this);
    _format->setEnabled(false);
    _format->setChecked(true);

    _wrapCellLine = new QCheckBox(QStringLiteral("Wrap line"), this);
    _wrapCellLine->setChecked(true);
    _cellView->setLineWrapMode(QTextEdit::WidgetWidth);
    _rawView->setLineWrapMode(QTextEdit::WidgetWidth);

    _htmlPreview = new QCheckBox(QStringLiteral("HTML preview"), this);
    _htmlPreview->setEnabled(false);
    _htmlPreview->setChecked(true);

    _cellSearch = new QLineEdit(this);
    _cellSearch->setPlaceholderText(QStringLiteral("Find in value..."));
    _cellSearch->setClearButtonEnabled(true);

    _rawSearch = new QLineEdit(this);
    _rawSearch->setPlaceholderText(QStringLiteral("Find in raw..."));
    _rawSearch->setClearButtonEnabled(true);

    _copyValueButton = new QToolButton(this);
    _copyValueButton->setIcon(icon_utils::copyIcon());
    _copyValueButton->setToolTip(QStringLiteral("Copy value"));
    _copyValueButton->setEnabled(false);

    _copyFormattedValueButton = new QToolButton(this);
    _copyFormattedValueButton->setIcon(icon_utils::copyFormattedIcon());
    _copyFormattedValueButton->setToolTip(QStringLiteral("Copy formatted value"));
    _copyFormattedValueButton->setEnabled(false);

    auto *activeCellTitle = new QLabel(QStringLiteral("Active Cell"), this);
    QFont detailsTitleFont = activeCellTitle->font();
    detailsTitleFont.setBold(true);
    activeCellTitle->setFont(detailsTitleFont);

    auto *cellTools = new QWidget(this);
    auto *cellToolsLayout = new QHBoxLayout(cellTools);
    cellToolsLayout->setContentsMargins(0, 0, 0, 0);
    cellToolsLayout->addWidget(activeCellTitle);
    cellToolsLayout->addWidget(_format);
    cellToolsLayout->addWidget(_wrapCellLine);
    cellToolsLayout->addWidget(_htmlPreview);
    cellToolsLayout->addWidget(_cellSearch, 1);
    cellToolsLayout->addWidget(_copyValueButton);
    cellToolsLayout->addWidget(_copyFormattedValueButton);
    cellToolsLayout->addStretch(1);
    cellTools->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    const int detailsToolsHeight = cellTools->sizeHint().height();
    cellTools->setFixedHeight(detailsToolsHeight);

    auto *activeCellPanel = new QWidget(this);
    auto *activeCellLayout = new QVBoxLayout(activeCellPanel);
    activeCellLayout->setContentsMargins(0, 0, 0, 0);
    activeCellLayout->setSpacing(0);
    activeCellLayout->addWidget(cellTools, 0);
    activeCellLayout->addWidget(_cellStack, 1);

    auto *rawCellTitle = new QLabel(QStringLiteral("Raw Cell"), this);
    rawCellTitle->setFont(detailsTitleFont);

    auto *rawCellTools = new QWidget(this);
    auto *rawCellToolsLayout = new QHBoxLayout(rawCellTools);
    rawCellToolsLayout->setContentsMargins(0, 0, 0, 0);
    rawCellToolsLayout->addWidget(rawCellTitle);
    rawCellToolsLayout->addWidget(_rawSearch, 1);
    rawCellToolsLayout->addStretch(1);
    rawCellTools->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    rawCellTools->setFixedHeight(detailsToolsHeight);

    auto *rawCellPanel = new QWidget(this);
    auto *rawCellLayout = new QVBoxLayout(rawCellPanel);
    rawCellLayout->setContentsMargins(0, 0, 0, 0);
    rawCellLayout->setSpacing(0);
    rawCellLayout->addWidget(rawCellTools, 0);
    rawCellLayout->addWidget(_rawView, 1);

    _detailsSplitter = new QSplitter(Qt::Horizontal, this);
    _detailsSplitter->addWidget(activeCellPanel);
    _detailsSplitter->addWidget(rawCellPanel);
    _detailsSplitter->setStretchFactor(0, 1);
    _detailsSplitter->setStretchFactor(1, 1);

    auto *cellPanel = new QWidget(this);
    auto *cellLayout = new QVBoxLayout(cellPanel);
    cellLayout->setContentsMargins(0, 0, 0, 0);
    cellLayout->setSpacing(0);
    cellLayout->addWidget(_detailsSplitter, 1);

    _mainSplitter = new QSplitter(Qt::Vertical, this);
    _mainSplitter->addWidget(_table);
    _mainSplitter->addWidget(cellPanel);
    _mainSplitter->setStretchFactor(0, 3);
    _mainSplitter->setStretchFactor(1, 1);

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    _filterPanel = new QWidget(this);
    _filterPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _filterPanel->setFixedHeight(qMax(_projectFilter->sizeHint().height(), _filter->sizeHint().height()));
    _projectFilter->setParent(_filterPanel);
    _appFilter->setParent(_filterPanel);
    _procNameFilter->setParent(_filterPanel);
    _moduleFilter->setParent(_filterPanel);
    _logNameFilter->setParent(_filterPanel);
    _tsFilterButton->setParent(_filterPanel);
    _levelFilter->setParent(_filterPanel);
    _queryFilter->setParent(_filterPanel);
    _msgFilter->setParent(_filterPanel);
    _filter->setParent(_filterPanel);

    layout->addWidget(_filterPanel);
    layout->addWidget(_mainSplitter);
    setCentralWidget(central);

    loadRecentFiles();

    auto *fileMenu = menuBar()->addMenu(QStringLiteral("File"));

    auto *openAction = new QAction(QStringLiteral("Open"), this);
    openAction->setShortcut(QKeySequence::Open);
    fileMenu->addAction(openAction);

    _recentFilesMenu = fileMenu->addMenu(QStringLiteral("Recent files..."));
    updateRecentFilesMenu();

    fileMenu->addSeparator();

    _openOriginalFileAction = fileMenu->addAction(QStringLiteral("Open original file location"));
    _openOriginalFileAction->setEnabled(false);

    fileMenu->addSeparator();

    auto *exitAction = fileMenu->addAction(QStringLiteral("Exit"));
    exitAction->setShortcut(QKeySequence::Quit);

    auto *viewMenu = menuBar()->addMenu(QStringLiteral("View"));
    auto *themeMenu = viewMenu->addMenu(QStringLiteral("Theme"));
    auto *themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);

    _lightThemeAction = themeMenu->addAction(QStringLiteral("Light"));
    _lightThemeAction->setCheckable(true);
    themeGroup->addAction(_lightThemeAction);

    _lightGrayThemeAction = themeMenu->addAction(QStringLiteral("LightGray"));
    _lightGrayThemeAction->setCheckable(true);
    themeGroup->addAction(_lightGrayThemeAction);

    _greyThemeAction = themeMenu->addAction(QStringLiteral("Grey"));
    _greyThemeAction->setCheckable(true);
    themeGroup->addAction(_greyThemeAction);

    _mediumGrayThemeAction = themeMenu->addAction(QStringLiteral("MediumGray"));
    _mediumGrayThemeAction->setCheckable(true);
    themeGroup->addAction(_mediumGrayThemeAction);

    _darkGreyThemeAction = themeMenu->addAction(QStringLiteral("DarkGrey"));
    _darkGreyThemeAction->setCheckable(true);
    themeGroup->addAction(_darkGreyThemeAction);

    _darkThemeAction = themeMenu->addAction(QStringLiteral("Dark"));
    _darkThemeAction->setCheckable(true);
    themeGroup->addAction(_darkThemeAction);

    auto *helpMenu = menuBar()->addMenu(QStringLiteral("Help"));
    auto *aboutAction = helpMenu->addAction(QStringLiteral("About"));

    _levelsStatusLabel = new QLabel(this);
    _levelsStatusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    statusBar()->addWidget(_levelsStatusLabel, 1);

    _statusLabel = new QLabel(this);
    _statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    statusBar()->addPermanentWidget(_statusLabel);

    _loadProgress = new QProgressBar(this);
    _loadProgress->setRange(0, 100);
    _loadProgress->setTextVisible(true);
    _loadProgress->setFixedWidth(180);
    _loadProgress->hide();
    statusBar()->addPermanentWidget(_loadProgress);

    connect(openAction, &QAction::triggered, this, [this] { openFile(); });
    connect(_openOriginalFileAction, &QAction::triggered, this, [this] {
        const QString fileName = _model->fileName();

        if (fileName.isEmpty()) {
            return;
        }

        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(fileName).absolutePath()));
    });
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(_lightThemeAction, &QAction::triggered, this, [this] {
        ThemeManager::applyTheme(qApp, ThemeManager::Theme::Light);
        QSettings settings(settingsFileName(), QSettings::IniFormat);
        settings.setValue(QStringLiteral("ui/theme"), ThemeManager::themeToString(ThemeManager::Theme::Light));
    });
    connect(_lightGrayThemeAction, &QAction::triggered, this, [this] {
        ThemeManager::applyTheme(qApp, ThemeManager::Theme::LightGray);
        QSettings settings(settingsFileName(), QSettings::IniFormat);
        settings.setValue(QStringLiteral("ui/theme"), ThemeManager::themeToString(ThemeManager::Theme::LightGray));
    });
    connect(_greyThemeAction, &QAction::triggered, this, [this] {
        ThemeManager::applyTheme(qApp, ThemeManager::Theme::Grey);
        QSettings settings(settingsFileName(), QSettings::IniFormat);
        settings.setValue(QStringLiteral("ui/theme"), ThemeManager::themeToString(ThemeManager::Theme::Grey));
    });
    connect(_mediumGrayThemeAction, &QAction::triggered, this, [this] {
        ThemeManager::applyTheme(qApp, ThemeManager::Theme::MediumGray);
        QSettings settings(settingsFileName(), QSettings::IniFormat);
        settings.setValue(QStringLiteral("ui/theme"), ThemeManager::themeToString(ThemeManager::Theme::MediumGray));
    });
    connect(_darkGreyThemeAction, &QAction::triggered, this, [this] {
        ThemeManager::applyTheme(qApp, ThemeManager::Theme::DarkGrey);
        QSettings settings(settingsFileName(), QSettings::IniFormat);
        settings.setValue(QStringLiteral("ui/theme"), ThemeManager::themeToString(ThemeManager::Theme::DarkGrey));
    });
    connect(_darkThemeAction, &QAction::triggered, this, [this] {
        ThemeManager::applyTheme(qApp, ThemeManager::Theme::Dark);
        QSettings settings(settingsFileName(), QSettings::IniFormat);
        settings.setValue(QStringLiteral("ui/theme"), ThemeManager::themeToString(ThemeManager::Theme::Dark));
    });
    connect(aboutAction, &QAction::triggered, this, [this] {
        QMessageBox::about(
            this,
            QStringLiteral("About VJson"),
            QStringLiteral(
                "<b>VJson</b><br>"
                "Qt Widgets viewer for JSONL and log files.<br><br>"
                "Features: filtering, level highlights, formatting for SQL/JSON/XML, and large-file load progress.<br><br>"
                "GitHub: <a href=\"https://github.com/skynowa/VJsonl\">https://github.com/skynowa/VJsonl</a><br><br>"
                "C++ standard: %1<br>"
                "Build date: %2<br>"
                "Build type: %3<br>"
                "OS: %4<br>"
                "Qt version: %5"
            )
                .arg(BuildInfo::cxxStandard())
                .arg(BuildInfo::buildDate())
                .arg(BuildInfo::buildType())
                .arg(BuildInfo::osInfo())
                .arg(QString::fromLatin1(qVersion()))
        );
    });

    connect(_filter, &QLineEdit::textChanged, this, [this](const QString &text) {
        _proxy->setTextFilter(text);
        updateStatus();
    });

    connect(_msgFilter, &QLineEdit::textChanged, this, [this](const QString &text) {
        _proxy->setMsgTextFilter(text);
        _table->scrollToTop();
        updateStatus();
    });

    auto applyComboFilters = [this](int index) {
        Q_UNUSED(index);
        applyFilters();
        updateTimestampFilterStatus();
        _table->scrollToTop();
        updateStatus();
    };

    connect(_levelFilter, &QComboBox::currentIndexChanged, this, applyComboFilters);
    connect(_projectFilter, &QComboBox::currentIndexChanged, this, applyComboFilters);
    connect(_appFilter, &QComboBox::currentIndexChanged, this, applyComboFilters);
    connect(_procNameFilter, &QComboBox::currentIndexChanged, this, applyComboFilters);
    connect(_moduleFilter, &QComboBox::currentIndexChanged, this, applyComboFilters);
    connect(_logNameFilter, &QComboBox::currentIndexChanged, this, applyComboFilters);
    connect(_queryFilter, &QLineEdit::textChanged, this, [this](const QString &text) {
        _proxy->setQueryFilter(text);
        _table->scrollToTop();
        updateStatus();
    });
    auto applyTimestampFilter = [this] {
        _tsFrom->setEnabled(_tsFromEnabled->isChecked());
        _tsTo->setEnabled(_tsToEnabled->isChecked());
        applyFilters();
        updateTimestampFilterStatus();
        _table->scrollToTop();
        updateStatus();
    };
    auto applyTimestampFrom = [this] {
        if (!_tsFromEnabled->isChecked()) {
            const QSignalBlocker blocker(_tsFromEnabled);
            _tsFromEnabled->setChecked(true);
        }

        _tsFrom->setEnabled(true);
        applyFilters();
        updateTimestampFilterStatus();
        _table->scrollToTop();
        updateStatus();
    };
    auto applyTimestampTo = [this] {
        if (!_tsToEnabled->isChecked()) {
            const QSignalBlocker blocker(_tsToEnabled);
            _tsToEnabled->setChecked(true);
        }

        _tsTo->setEnabled(true);
        applyFilters();
        updateTimestampFilterStatus();
        _table->scrollToTop();
        updateStatus();
    };
    connect(_tsFromEnabled, &QCheckBox::toggled, this, applyTimestampFilter);
    connect(_tsToEnabled, &QCheckBox::toggled, this, applyTimestampFilter);
    connect(_tsFrom, &QDateTimeEdit::dateTimeChanged, this, applyTimestampFrom);
    connect(_tsTo, &QDateTimeEdit::dateTimeChanged, this, applyTimestampTo);
    connect(_table->horizontalHeader(), &QHeaderView::sectionResized, this, [this] {
        updateFilterGeometry();
    });
    connect(_table->horizontalHeader(), &QHeaderView::sectionMoved, this, [this] {
        updateFilterGeometry();
    });
    connect(_table->horizontalScrollBar(), &QScrollBar::valueChanged, this, [this] {
        updateFilterGeometry();
    });

    connect(
        _table->selectionModel(),
        &QItemSelectionModel::currentChanged,
        this,
        &MainWindow::onCurrentChanged
    );

    connect(
        _table->selectionModel(),
        &QItemSelectionModel::currentChanged,
        _table->viewport(),
        qOverload<>(&QWidget::update)
    );

    connect(_format, &QCheckBox::toggled, this, [this] {
        updateCellView(_table->currentIndex());
    });

    connect(_wrapCellLine, &QCheckBox::toggled, this, [this](bool checked) {
        _cellView->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
        _rawView->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
    });

    connect(_htmlPreview, &QCheckBox::toggled, this, [this] {
        updateCellView(_table->currentIndex());
    });

    connect(_cellSearch, &QLineEdit::textChanged, this, [this] {
        findInCellView();
    });

    connect(_rawSearch, &QLineEdit::textChanged, this, [this] {
        findInRawView();
    });

    connect(_copyValueButton, &QToolButton::clicked, this, &MainWindow::copyActiveCellValue);
    connect(_copyFormattedValueButton, &QToolButton::clicked, this, &MainWindow::copyFormattedCellValue);

    new QShortcut(QKeySequence::Find, this, [this] {
        _filter->setFocus();
        _filter->selectAll();
    });

    setWindowTitle(QStringLiteral("VJson"));
    resize(1300, 850);

    QSettings settings(settingsFileName(), QSettings::IniFormat);
    const auto theme = ThemeManager::themeFromString(settings.value(QStringLiteral("ui/theme"), QStringLiteral("light")).toString());
    ThemeManager::applyTheme(qApp, theme);
    _lightThemeAction->setChecked(theme == ThemeManager::Theme::Light);
    _lightGrayThemeAction->setChecked(theme == ThemeManager::Theme::LightGray);
    _greyThemeAction->setChecked(theme == ThemeManager::Theme::Grey);
    _mediumGrayThemeAction->setChecked(theme == ThemeManager::Theme::MediumGray);
    _darkGreyThemeAction->setChecked(theme == ThemeManager::Theme::DarkGrey);
    _darkThemeAction->setChecked(theme == ThemeManager::Theme::Dark);
    restoreGeometry(settings.value(QStringLiteral("window/geometry")).toByteArray());
    restorePanelLayout();
    restoreColumnWidths();
    QTimer::singleShot(0, this, [this] {
        restorePanelLayout();
        restoreColumnWidths();
        updateFilterGeometry();
    });

    updateFilterGeometry();
    updateStatus();
}
//-------------------------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(settingsFileName(), QSettings::IniFormat);
    settings.setValue(QStringLiteral("window/geometry"), saveGeometry());
    savePanelLayout();
    saveColumnWidths();
    QMainWindow::closeEvent(event);
}
//-------------------------------------------------------------------------------------------------
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (
        (watched == _table->viewport() || watched == _table->horizontalHeader())
        && event->type() == QEvent::Resize
    ) {
        QTimer::singleShot(0, this, [this] {
            updateFilterGeometry();
        });
    }

    return QMainWindow::eventFilter(watched, event);
}
//-------------------------------------------------------------------------------------------------
void MainWindow::openFile()
{
    const QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open JSONL",
        openDirectory(),
        "JSONL files (*.jsonl *.log *.txt);;All files (*)"
    );

    if (fileName.isEmpty()) {
        return;
    }

    openFile(fileName);
}
//-------------------------------------------------------------------------------------------------
void MainWindow::openFile(const QString &fileName)
{
    saveColumnWidths();

    QString error;
    constexpr qint64 progressThresholdBytes = 50LL * 1024 * 1024;
    const qint64 fileSize = QFileInfo(fileName).size();
    const bool showProgress = fileSize > progressThresholdBytes;

    if (showProgress) {
        _loadProgress->setValue(0);
        _loadProgress->show();
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    const bool ok = _model->loadFile(fileName, &error, [this, showProgress](qint64 current, qint64 total) {
        if (!showProgress || total <= 0) {
            return;
        }

        _loadProgress->setValue(static_cast<int>((current * 100) / total));
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    });
    QApplication::restoreOverrideCursor();
    _loadProgress->hide();

    if (!ok) {
        QMessageBox::critical(this, QStringLiteral("Open failed"), error);
        return;
    }

    _cellView->clear();
    _rawView->clear();
    _htmlPreviewView->clear();
    _format->setEnabled(false);
    _htmlPreview->setEnabled(false);
    updateColumnFilterItems(_logNameFilter, QStringLiteral("log_name"), QStringLiteral("All log names"));
    updateColumnFilterItems(_projectFilter, QStringLiteral("project"), QStringLiteral("All projects"));
    updateColumnFilterItems(_appFilter, QStringLiteral("app"), QStringLiteral("All apps"));
    updateColumnFilterItems(_procNameFilter, QStringLiteral("proc_name"), QStringLiteral("All proc names"));
    updateColumnFilterItems(_moduleFilter, QStringLiteral("module"), QStringLiteral("All modules"));
    updateTimestampFilterBounds();
    _table->sortByColumn(-1, Qt::AscendingOrder);
    _table->resizeColumnsToContents();

    constexpr int maxColumnWidth = 360;

    for (int column = 0; column < _table->model()->columnCount(); ++column) {
        const int currentWidth = _table->columnWidth(column);

        if (currentWidth > maxColumnWidth) {
            _table->setColumnWidth(column, maxColumnWidth);
        }
    }

    restoreColumnWidths();
    updateFilterGeometry();

    addRecentFile(fileName);
    saveOpenDirectory(fileName);
    _openOriginalFileAction->setEnabled(true);
    setWindowTitle(QStringLiteral("VJson - %1").arg(QFileInfo(fileName).fileName()));
    updateStatus();
}
//-------------------------------------------------------------------------------------------------
void MainWindow::onCurrentChanged(const QModelIndex &current)
{
    if (!current.isValid()) {
        _activeCellValue.clear();
        _formattedCellValue.clear();
        _cellView->clear();
        _rawView->clear();
        _cellJsonHighlighter->setMode(JsonSyntaxHighlighter::Mode::None);
        _rawJsonHighlighter->setMode(JsonSyntaxHighlighter::Mode::None);
        _htmlPreviewView->clear();
        _format->setEnabled(false);
        _htmlPreview->setEnabled(false);
        _copyValueButton->setEnabled(false);
        _copyFormattedValueButton->setEnabled(false);
        updateStatus();
        return;
    }

    updateCellView(current);
    updateStatus();
}
//-------------------------------------------------------------------------------------------------
void MainWindow::updateCellView(const QModelIndex &current)
{
    if (!current.isValid()) {
        _activeCellValue.clear();
        _formattedCellValue.clear();
        _cellView->clear();
        _rawView->clear();
        _cellJsonHighlighter->setMode(JsonSyntaxHighlighter::Mode::None);
        _rawJsonHighlighter->setMode(JsonSyntaxHighlighter::Mode::None);
        _htmlPreviewView->clear();
        _format->setEnabled(false);
        _htmlPreview->setEnabled(false);
        _copyValueButton->setEnabled(false);
        _copyFormattedValueButton->setEnabled(false);
        return;
    }

    const QModelIndex sourceIndex = _proxy->mapToSource(current);
    const auto *record = _model->recordAt(sourceIndex.row());
    const QString columnName = current.model()->headerData(current.column(), Qt::Horizontal, Qt::DisplayRole).toString();
    const QString text = current.data(Qt::DisplayRole).toString();
    _activeCellValue = text;
    const QString previewText = columnName == QStringLiteral("backtrace") ? demangle_utils::demangleSymbols(text) : text;
    bool canFormat = false;
    const QString formatted = CodeFormatter::formatFragments(previewText, &canFormat);
    _formattedCellValue = canFormat ? formatted : QString();
    const QString displayText = canFormat && _format->isChecked() ? formatted : previewText;
    const bool canPreviewHtml = html_utils::looksLikeHtml(previewText);
    bool canFormatRaw = false;
    const QString rawText = record != nullptr ? record->raw : QString();
    const QString formattedRaw = CodeFormatter::formatFragments(rawText, &canFormatRaw);

    _format->setEnabled(canFormat);
    _htmlPreview->setEnabled(canPreviewHtml);
    _copyValueButton->setEnabled(true);
    _copyFormattedValueButton->setEnabled(canFormat);

    _cellView->setPlainText(displayText);
    _htmlPreviewView->setHtml(previewText);
    _rawView->setPlainText(canFormatRaw ? formattedRaw : rawText);
    _cellStack->setCurrentWidget(canPreviewHtml && _htmlPreview->isChecked() ? _htmlPreviewView : _cellView);
    const auto cellHighlightMode =
        _cellStack->currentWidget() != _cellView
            ? JsonSyntaxHighlighter::Mode::None
            : columnName == QStringLiteral("backtrace") ? JsonSyntaxHighlighter::Mode::Backtrace
            : CodeFormatter::looksLikeJson(displayText) ? JsonSyntaxHighlighter::Mode::Json
            : CodeFormatter::looksLikeSql(displayText)  ? JsonSyntaxHighlighter::Mode::Sql
            : html_utils::looksLikeHtml(displayText)     ? JsonSyntaxHighlighter::Mode::Html
            : CodeFormatter::looksLikeXml(displayText)  ? JsonSyntaxHighlighter::Mode::Xml
                                                        : JsonSyntaxHighlighter::Mode::None;
    const auto rawHighlightMode =
        CodeFormatter::looksLikeJson(_rawView->toPlainText()) ? JsonSyntaxHighlighter::Mode::Json
        : CodeFormatter::looksLikeSql(_rawView->toPlainText())  ? JsonSyntaxHighlighter::Mode::Sql
        : html_utils::looksLikeHtml(_rawView->toPlainText())     ? JsonSyntaxHighlighter::Mode::Html
        : CodeFormatter::looksLikeXml(_rawView->toPlainText())  ? JsonSyntaxHighlighter::Mode::Xml
                                                                : JsonSyntaxHighlighter::Mode::None;
    _cellJsonHighlighter->setMode(cellHighlightMode);
    _rawJsonHighlighter->setMode(rawHighlightMode);
    findInCellView();
    findInRawView();
}
//-------------------------------------------------------------------------------------------------
void MainWindow::copyActiveCellValue()
{
    QApplication::clipboard()->setText(_activeCellValue);
}
//-------------------------------------------------------------------------------------------------
void MainWindow::copyFormattedCellValue()
{
    QApplication::clipboard()->setText(_formattedCellValue);
}
//-------------------------------------------------------------------------------------------------
void MainWindow::findInTextView(QLineEdit *search, QTextEdit *view)
{
    const QString text = search->text();
    const int matchCount = text_search_utils::highlightAll(view, text);
    search->setStyleSheet({});

    if (!text.isEmpty() && matchCount == 0) {
        search->setStyleSheet(QStringLiteral("background-color: #ffdede;"));
    }
}

//-------------------------------------------------------------------------------------------------
void MainWindow::findInCellView()
{
    auto *activeTextView = qobject_cast<QTextEdit *>(_cellStack->currentWidget());
    findInTextView(_cellSearch, activeTextView);
}

//-------------------------------------------------------------------------------------------------
void MainWindow::findInRawView()
{
    findInTextView(_rawSearch, _rawView);
}
//-------------------------------------------------------------------------------------------------
void MainWindow::addRecentFile(const QString &fileName)
{
    _recentFiles.removeAll(fileName);
    _recentFiles.prepend(fileName);

    constexpr int maxRecentFiles = 50;

    while (_recentFiles.size() > maxRecentFiles) {
        _recentFiles.removeLast();
    }

    QSettings settings(settingsFileName(), QSettings::IniFormat);
    settings.setValue(QStringLiteral("recentFiles"), _recentFiles);
    updateRecentFilesMenu();
}
//-------------------------------------------------------------------------------------------------
void MainWindow::loadRecentFiles()
{
    QSettings settings(settingsFileName(), QSettings::IniFormat);
    _recentFiles = settings.value(QStringLiteral("recentFiles")).toStringList();
}
//-------------------------------------------------------------------------------------------------
void MainWindow::updateRecentFilesMenu()
{
    if (_recentFilesMenu == nullptr) {
        return;
    }

    _recentFilesMenu->clear();

    if (_recentFiles.isEmpty()) {
        auto *emptyAction = _recentFilesMenu->addAction(QStringLiteral("No recent files"));
        emptyAction->setEnabled(false);
        return;
    }

    for (const QString &fileName : std::as_const(_recentFiles)) {
        auto *action = _recentFilesMenu->addAction(QFileInfo(fileName).fileName());
        action->setToolTip(fileName);

        connect(action, &QAction::triggered, this, [this, fileName] {
            openFile(fileName);
        });
    }

    _recentFilesMenu->addSeparator();

    auto *clearAction = _recentFilesMenu->addAction(QStringLiteral("Clear recent files"));
    connect(clearAction, &QAction::triggered, this, [this] {
        _recentFiles.clear();
        QSettings settings(settingsFileName(), QSettings::IniFormat);
        settings.setValue(QStringLiteral("recentFiles"), _recentFiles);
        updateRecentFilesMenu();
    });
}
//-------------------------------------------------------------------------------------------------
void MainWindow::updateColumnFilterItems(QComboBox *filter, const QString &columnName, const QString &allLabel)
{
    if (filter == nullptr) {
        return;
    }

    const QString selectedValue = filter_utils::selectedFilterValue(filter);
    QStringList values;

    for (int row = 0; row < _model->rowCount(); ++row) {
        const auto *record = _model->recordAt(row);

        if (record == nullptr) {
            continue;
        }

        const QString value = record->value(columnName);

        if (!value.isEmpty() && !values.contains(value, Qt::CaseInsensitive)) {
            values.push_back(value);
        }
    }

    values.sort(Qt::CaseInsensitive);

    QSignalBlocker blocker(filter);
    filter->clear();
    filter->addItem(allLabel, QString());

    for (const QString &value : values) {
        filter->addItem(value, value);
    }

    const int selectedIndex = filter->findData(selectedValue);
    filter->setCurrentIndex(selectedIndex >= 0 ? selectedIndex : 0);
    applyFilters();
}
//-------------------------------------------------------------------------------------------------
void MainWindow::applyFilters()
{
    _proxy->setProjectFilter(filter_utils::selectedFilterValue(_projectFilter));
    _proxy->setAppFilter(filter_utils::selectedFilterValue(_appFilter));
    _proxy->setProcNameFilter(filter_utils::selectedFilterValue(_procNameFilter));
    _proxy->setModuleFilter(filter_utils::selectedFilterValue(_moduleFilter));
    _proxy->setLogNameFilter(filter_utils::selectedFilterValue(_logNameFilter));
    _proxy->setLevelFilter(filter_utils::selectedFilterValue(_levelFilter).toLower());

    QDateTime from = _tsFrom->dateTime();
    QDateTime to = _tsTo->dateTime();

    if (_tsFromEnabled->isChecked() && _tsToEnabled->isChecked() && from > to) {
        std::swap(from, to);
    }

    _proxy->setTimestampRange(
        from,
        _tsFromEnabled->isChecked(),
        to,
        _tsToEnabled->isChecked()
    );
}

//-------------------------------------------------------------------------------------------------
void MainWindow::updateTimestampFilterStatus()
{
    if (_tsFilterButton == nullptr) {
        return;
    }

    const bool hasFrom = _tsFromEnabled->isChecked();
    const bool hasTo = _tsToEnabled->isChecked();
    QString text = QStringLiteral("off");
    QString tooltip = QStringLiteral("Timestamp filter is off");

    if (hasFrom && hasTo) {
        text = QStringLiteral("range");
        tooltip = QStringLiteral("ts from %1\nts to %2").arg(_tsFrom->text(), _tsTo->text());
    } else if (hasFrom) {
        text = QStringLiteral("from");
        tooltip = QStringLiteral("ts from %1").arg(_tsFrom->text());
    } else if (hasTo) {
        text = QStringLiteral("to");
        tooltip = QStringLiteral("ts to %1").arg(_tsTo->text());
    }

    _tsFilterButton->setText(text);
    _tsFilterButton->setToolTip(tooltip);
}

//-------------------------------------------------------------------------------------------------
void MainWindow::updateFilterGeometry()
{
    if (_filterPanel == nullptr || _table == nullptr || _table->model() == nullptr) {
        return;
    }

    const QRect viewportRect = _table->viewport()->geometry();
    const int panelHeight = _filterPanel->height();

    auto placeFilter = [&](QWidget *filter, const QString &columnName) {
        if (filter == nullptr) {
            return;
        }

        const int column = filter_utils::columnByName(_table->model(), columnName);

        if (column < 0 || _table->isColumnHidden(column)) {
            filter->hide();
            return;
        }

        const int sectionX = _table->horizontalHeader()->sectionViewportPosition(column);
        const int sectionWidth = _table->horizontalHeader()->sectionSize(column);
        const int x = viewportRect.x() + sectionX;
        const int left = qMax(x, viewportRect.left());
        const int right = qMin(x + sectionWidth, viewportRect.right() + 1);
        const int width = right - left;

        if (width <= 0) {
            filter->hide();
            return;
        }

        filter->setGeometry(left, 0, width, panelHeight);
        filter->show();
    };

    placeFilter(_projectFilter, QStringLiteral("project"));
    placeFilter(_appFilter, QStringLiteral("app"));
    placeFilter(_procNameFilter, QStringLiteral("proc_name"));
    placeFilter(_moduleFilter, QStringLiteral("module"));
    placeFilter(_logNameFilter, QStringLiteral("log_name"));
    placeFilter(_tsFilterButton, QStringLiteral("ts"));
    placeFilter(_levelFilter, QStringLiteral("level"));
    placeFilter(_msgFilter, QStringLiteral("msg"));
    placeFilter(_queryFilter, QStringLiteral("query"));
    placeFilter(_filter, QStringLiteral("raw"));
}

//-------------------------------------------------------------------------------------------------
void MainWindow::updateTimestampFilterBounds()
{
    QDateTime minTimestamp;
    QDateTime maxTimestamp;

    for (int row = 0; row < _model->rowCount(); ++row) {
        const auto *record = _model->recordAt(row);

        if (record == nullptr) {
            continue;
        }

        const QDateTime timestamp = datetime_utils::parseTimestamp(record->value(QStringLiteral("ts")));

        if (!timestamp.isValid()) {
            continue;
        }

        if (!minTimestamp.isValid() || timestamp < minTimestamp) {
            minTimestamp = timestamp;
        }

        if (!maxTimestamp.isValid() || timestamp > maxTimestamp) {
            maxTimestamp = timestamp;
        }
    }

    if (!minTimestamp.isValid() || !maxTimestamp.isValid()) {
        minTimestamp = QDateTime::currentDateTime();
        maxTimestamp = minTimestamp;
    }

    const QSignalBlocker fromEnabledBlocker(_tsFromEnabled);
    const QSignalBlocker toEnabledBlocker(_tsToEnabled);
    const QSignalBlocker fromBlocker(_tsFrom);
    const QSignalBlocker toBlocker(_tsTo);

    _tsFromEnabled->setChecked(false);
    _tsToEnabled->setChecked(false);
    _tsFrom->setEnabled(false);
    _tsTo->setEnabled(false);
    _tsFrom->setMinimumDateTime(minTimestamp);
    _tsFrom->setMaximumDateTime(maxTimestamp);
    _tsTo->setMinimumDateTime(minTimestamp);
    _tsTo->setMaximumDateTime(maxTimestamp);
    _tsFrom->setDateTime(minTimestamp);
    _tsTo->setDateTime(maxTimestamp);
    updateTimestampFilterStatus();
    applyFilters();
}

//-------------------------------------------------------------------------------------------------
QString MainWindow::openDirectory() const
{
    QSettings settings(settingsFileName(), QSettings::IniFormat);
    const QString defaultDirectory = QDir::homePath() + QStringLiteral("/tmp");
    const QString directory = settings.value(QStringLiteral("open/directory"), defaultDirectory).toString();

    return QDir(directory).exists() ? directory : defaultDirectory;
}
//-------------------------------------------------------------------------------------------------
void MainWindow::saveOpenDirectory(const QString &fileName) const
{
    QSettings settings(settingsFileName(), QSettings::IniFormat);
    settings.setValue(QStringLiteral("open/directory"), QFileInfo(fileName).absolutePath());
}
//-------------------------------------------------------------------------------------------------
void MainWindow::saveColumnWidths() const
{
    if (_model->fileName().isEmpty()) {
        return;
    }

    QSettings settings(settingsFileName(), QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("columns"));

    for (int column = 0; column < _table->model()->columnCount(); ++column) {
        const QString columnName = _table->model()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();

        if (!columnName.isEmpty()) {
            settings.setValue(columnName, _table->columnWidth(column));
        }
    }

    settings.endGroup();
}
//-------------------------------------------------------------------------------------------------
void MainWindow::restoreColumnWidths()
{
    QSettings settings(settingsFileName(), QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("columns"));

    for (int column = 0; column < _table->model()->columnCount(); ++column) {
        const QString columnName = _table->model()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
        bool ok = false;
        const int width = settings.value(columnName).toInt(&ok);

        if (ok && width > 0) {
            _table->setColumnWidth(column, width);
        }
    }

    settings.endGroup();
}
//-------------------------------------------------------------------------------------------------
void MainWindow::savePanelLayout() const
{
    QSettings settings(settingsFileName(), QSettings::IniFormat);
    settings.setValue(QStringLiteral("splitters/main"), _mainSplitter->saveState());
    settings.setValue(QStringLiteral("splitters/details"), _detailsSplitter->saveState());
}
//-------------------------------------------------------------------------------------------------
void MainWindow::restorePanelLayout()
{
    QSettings settings(settingsFileName(), QSettings::IniFormat);
    _mainSplitter->restoreState(settings.value(QStringLiteral("splitters/main")).toByteArray());
    _detailsSplitter->restoreState(settings.value(QStringLiteral("splitters/details")).toByteArray());
}
//-------------------------------------------------------------------------------------------------
void MainWindow::updateStatus()
{
    const QString fileName = _model->fileName();
    const QString fileSize = fileName.isEmpty()
        ? QStringLiteral("-")
        : file_utils::humanFileSize(QFileInfo(fileName).size());
    const QModelIndex current = _table->currentIndex();
    QString currentLine = QStringLiteral("-");

    if (current.isValid()) {
        const QModelIndex sourceIndex = _proxy->mapToSource(current);
        const auto *record = _model->recordAt(sourceIndex.row());

        if (record != nullptr) {
            currentLine = QString::number(record->lineNo);
        }
    }

    QString levelsText = QStringLiteral("-");
    QString levelsHtml;
    const QMap<QString, int> levelCounts = _model->levelCounts();

    if (!levelCounts.isEmpty()) {
        QStringList levelParts;
        QStringList levelHtmlParts;
        const QStringList orderedLevels {
            QStringLiteral("fatal"),
            QStringLiteral("error"),
            QStringLiteral("warn"),
            QStringLiteral("warning"),
            QStringLiteral("debug"),
            QStringLiteral("info"),
            QStringLiteral("trace")
        };

        for (const QString &level : orderedLevels) {
            const int count = levelCounts.value(level);

            if (count > 0) {
                levelParts.push_back(QStringLiteral("%1=%2").arg(level).arg(count));
                levelHtmlParts.push_back(LogLevelStyle::levelCounterHtml(level, count));
            }
        }

        for (auto it = levelCounts.cbegin(); it != levelCounts.cend(); ++it) {
            if (!orderedLevels.contains(it.key())) {
                levelParts.push_back(QStringLiteral("%1=%2").arg(it.key()).arg(it.value()));
                levelHtmlParts.push_back(LogLevelStyle::levelCounterHtml(it.key(), it.value()));
            }
        }

        levelsText = levelParts.join(QStringLiteral(", "));
        levelsHtml = levelHtmlParts.join(QStringLiteral("&nbsp;&nbsp;&nbsp;"));
    }

    _levelsStatusLabel->setText(levelsHtml.isEmpty() ? QStringLiteral("-") : levelsHtml);
    _levelsStatusLabel->setToolTip(QStringLiteral("levels: %1").arg(levelsText));

    _statusLabel->setText(
        QStringLiteral("current: %1 / %2, invalid: %3, file: %4")
            .arg(currentLine)
            .arg(_model->rowCount())
            .arg(_model->invalidRowsCount())
            .arg(fileSize)
    );
}
//-------------------------------------------------------------------------------------------------
