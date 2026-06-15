/**
 * \file  MainWindow.cpp
 * \brief Implements the main Qt Widgets window and user interactions.
 */


#include "MainWindow.h"

#include "AppSettings.h"
#include "BuildInfo.h"
#include "CodeFormatter.h"
#include "Delegates.h"
#include "DemangleUtils.h"
#include "FileUtils.h"
#include "HtmlUtils.h"
#include "JsonSyntaxHighlighter.h"
#include "LogFilterProxyModel.h"
#include "LogLevelStyle.h"

#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDesktopServices>
#include <QDir>
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
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QSignalBlocker>
#include <QTableView>
#include <QTextBrowser>
#include <QTextCursor>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
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

    _levelFilter = new QComboBox(this);
    _levelFilter->addItem(QStringLiteral("All levels"), QString());
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("fatal")), QStringLiteral("Fatal"), QStringLiteral("fatal"));
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("error")), QStringLiteral("Error"), QStringLiteral("error"));
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("warn")), QStringLiteral("Warn"), QStringLiteral("warn"));
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("warning")), QStringLiteral("Warning"), QStringLiteral("warning"));
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("debug")), QStringLiteral("Debug"), QStringLiteral("debug"));
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("info")), QStringLiteral("Info"), QStringLiteral("info"));
    _levelFilter->addItem(LogLevelStyle::iconForLevel(QStringLiteral("trace")), QStringLiteral("Trace"), QStringLiteral("trace"));

    _logNameFilter = new QComboBox(this);
    _logNameFilter->addItem(QStringLiteral("All log names"), QString());
    _logNameFilter->setMinimumWidth(160);

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

    auto *cellTools = new QWidget(this);
    auto *cellToolsLayout = new QHBoxLayout(cellTools);
    cellToolsLayout->setContentsMargins(0, 0, 0, 0);
    cellToolsLayout->addWidget(_format);
    cellToolsLayout->addWidget(_wrapCellLine);
    cellToolsLayout->addWidget(_htmlPreview);
    cellToolsLayout->addWidget(_cellSearch, 1);
    cellToolsLayout->addStretch(1);
    cellTools->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cellTools->setFixedHeight(cellTools->sizeHint().height());

    auto *cellPanel = new QWidget(this);
    auto *cellLayout = new QVBoxLayout(cellPanel);
    cellLayout->setContentsMargins(0, 0, 0, 0);
    cellLayout->addWidget(cellTools, 0);

    _detailsSplitter = new QSplitter(Qt::Horizontal, this);
    _detailsSplitter->addWidget(_cellStack);
    _detailsSplitter->addWidget(_rawView);
    _detailsSplitter->setStretchFactor(0, 1);
    _detailsSplitter->setStretchFactor(1, 1);

    cellLayout->addWidget(_detailsSplitter, 1);

    _mainSplitter = new QSplitter(Qt::Vertical, this);
    _mainSplitter->addWidget(_table);
    _mainSplitter->addWidget(cellPanel);
    _mainSplitter->setStretchFactor(0, 3);
    _mainSplitter->setStretchFactor(1, 1);

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    auto *filterPanel = new QWidget(this);
    auto *filterLayout = new QHBoxLayout(filterPanel);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->addWidget(_logNameFilter);
    filterLayout->addWidget(_levelFilter);
    filterLayout->addWidget(_filter, 1);

    layout->addWidget(filterPanel);
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

    auto *helpMenu = menuBar()->addMenu(QStringLiteral("Help"));
    auto *aboutAction = helpMenu->addAction(QStringLiteral("About"));

    _statusLabel = new QLabel(this);
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

    connect(_levelFilter, &QComboBox::currentIndexChanged, this, [this](int index) {
        _proxy->setLevelFilter(_levelFilter->itemData(index).toString());
        updateStatus();
    });

    connect(_logNameFilter, &QComboBox::currentIndexChanged, this, [this](int index) {
        _proxy->setLogNameFilter(_logNameFilter->itemData(index).toString());
        updateStatus();
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

    new QShortcut(QKeySequence::Find, this, [this] {
        _filter->setFocus();
        _filter->selectAll();
    });

    setWindowTitle(QStringLiteral("VJson"));
    resize(1300, 850);

    QSettings settings(settingsFileName(), QSettings::IniFormat);
    restoreGeometry(settings.value(QStringLiteral("window/geometry")).toByteArray());
    restorePanelLayout();
    restoreColumnWidths();
    QTimer::singleShot(0, this, [this] {
        restorePanelLayout();
        restoreColumnWidths();
    });

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
    updateLogNameFilterItems();
    _table->sortByColumn(0, Qt::AscendingOrder);
    _table->resizeColumnsToContents();

    constexpr int maxColumnWidth = 360;

    for (int column = 0; column < _table->model()->columnCount(); ++column) {
        const int currentWidth = _table->columnWidth(column);

        if (currentWidth > maxColumnWidth) {
            _table->setColumnWidth(column, maxColumnWidth);
        }
    }

    restoreColumnWidths();

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
        _cellView->clear();
        _rawView->clear();
        _cellJsonHighlighter->setMode(JsonSyntaxHighlighter::Mode::None);
        _rawJsonHighlighter->setMode(JsonSyntaxHighlighter::Mode::None);
        _htmlPreviewView->clear();
        _format->setEnabled(false);
        _htmlPreview->setEnabled(false);
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
        _cellView->clear();
        _rawView->clear();
        _cellJsonHighlighter->setMode(JsonSyntaxHighlighter::Mode::None);
        _rawJsonHighlighter->setMode(JsonSyntaxHighlighter::Mode::None);
        _htmlPreviewView->clear();
        _format->setEnabled(false);
        _htmlPreview->setEnabled(false);
        return;
    }

    const QModelIndex sourceIndex = _proxy->mapToSource(current);
    const auto *record = _model->recordAt(sourceIndex.row());
    const QString columnName = current.model()->headerData(current.column(), Qt::Horizontal, Qt::DisplayRole).toString();
    const QString text = current.data(Qt::DisplayRole).toString();
    const QString previewText = columnName == QStringLiteral("backtrace") ? demangleSymbols(text) : text;
    bool canFormat = false;
    const QString formatted = CodeFormatter::formatFragments(previewText, &canFormat);
    const QString displayText = canFormat && _format->isChecked() ? formatted : previewText;
    const bool canPreviewHtml = HtmlUtils::looksLikeHtml(previewText);
    bool canFormatRaw = false;
    const QString rawText = record != nullptr ? record->raw : QString();
    const QString formattedRaw = CodeFormatter::formatFragments(rawText, &canFormatRaw);

    _format->setEnabled(canFormat);
    _htmlPreview->setEnabled(canPreviewHtml);

    _cellView->setPlainText(displayText);
    _htmlPreviewView->setHtml(previewText);
    _rawView->setPlainText(canFormatRaw ? formattedRaw : rawText);
    _cellStack->setCurrentWidget(canPreviewHtml && _htmlPreview->isChecked() ? _htmlPreviewView : _cellView);
    const auto cellHighlightMode =
        _cellStack->currentWidget() != _cellView
            ? JsonSyntaxHighlighter::Mode::None
            : CodeFormatter::looksLikeJson(displayText) ? JsonSyntaxHighlighter::Mode::Json
            : CodeFormatter::looksLikeSql(displayText)  ? JsonSyntaxHighlighter::Mode::Sql
            : HtmlUtils::looksLikeHtml(displayText)     ? JsonSyntaxHighlighter::Mode::Html
            : CodeFormatter::looksLikeXml(displayText)  ? JsonSyntaxHighlighter::Mode::Xml
                                                        : JsonSyntaxHighlighter::Mode::None;
    const auto rawHighlightMode =
        CodeFormatter::looksLikeJson(_rawView->toPlainText()) ? JsonSyntaxHighlighter::Mode::Json
        : CodeFormatter::looksLikeSql(_rawView->toPlainText())  ? JsonSyntaxHighlighter::Mode::Sql
        : HtmlUtils::looksLikeHtml(_rawView->toPlainText())     ? JsonSyntaxHighlighter::Mode::Html
        : CodeFormatter::looksLikeXml(_rawView->toPlainText())  ? JsonSyntaxHighlighter::Mode::Xml
                                                                : JsonSyntaxHighlighter::Mode::None;
    _cellJsonHighlighter->setMode(cellHighlightMode);
    _rawJsonHighlighter->setMode(rawHighlightMode);
    findInCellView();
}
//-------------------------------------------------------------------------------------------------
void MainWindow::findInCellView()
{
    const QString text = _cellSearch->text();
    auto *activeTextView = qobject_cast<QTextEdit *>(_cellStack->currentWidget());
    _cellSearch->setStyleSheet({});

    if (text.isEmpty() || activeTextView == nullptr) {
        return;
    }

    QTextCursor cursor = activeTextView->textCursor();
    cursor.movePosition(QTextCursor::Start);
    activeTextView->setTextCursor(cursor);

    if (!activeTextView->find(text)) {
        _cellSearch->setStyleSheet(QStringLiteral("background-color: #ffdede;"));
    }
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
void MainWindow::updateLogNameFilterItems()
{
    const QString selectedLogName = _logNameFilter->currentData().toString();
    QStringList logNames;

    for (int row = 0; row < _model->rowCount(); ++row) {
        const auto *record = _model->recordAt(row);

        if (record == nullptr) {
            continue;
        }

        const QString logName = record->value(QStringLiteral("log_name"));

        if (!logName.isEmpty() && !logNames.contains(logName, Qt::CaseInsensitive)) {
            logNames.push_back(logName);
        }
    }

    logNames.sort(Qt::CaseInsensitive);

    QSignalBlocker blocker(_logNameFilter);
    _logNameFilter->clear();
    _logNameFilter->addItem(QStringLiteral("All log names"), QString());

    for (const QString &logName : logNames) {
        _logNameFilter->addItem(logName, logName);
    }

    const int selectedIndex = _logNameFilter->findData(selectedLogName);
    _logNameFilter->setCurrentIndex(selectedIndex >= 0 ? selectedIndex : 0);
    _proxy->setLogNameFilter(_logNameFilter->currentData().toString());
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
        : humanFileSize(QFileInfo(fileName).size());
    const QModelIndex current = _table->currentIndex();
    QString currentLine = QStringLiteral("-");

    if (current.isValid()) {
        const QModelIndex sourceIndex = _proxy->mapToSource(current);
        const auto *record = _model->recordAt(sourceIndex.row());

        if (record != nullptr) {
            currentLine = QString::number(record->lineNo);
        }
    }

    _statusLabel->setText(
        QStringLiteral("current: %1 / %2, invalid: %3, file: %4")
            .arg(currentLine)
            .arg(_model->rowCount())
            .arg(_model->invalidRowsCount())
            .arg(fileSize)
    );
}
//-------------------------------------------------------------------------------------------------
