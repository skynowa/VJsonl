/**
 * \file  ThemeManager.cpp
 * \brief Implements application theme helpers.
 */


#include "ThemeManager.h"

#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QStyle>

//-------------------------------------------------------------------------------------------------
namespace ThemeManager
{
Theme
themeFromString(
    const QString &value
)
{
    if (value.compare(QStringLiteral("lightgray"), Qt::CaseInsensitive) == 0) {
        return Theme::LightGray;
    }

    if (value.compare(QStringLiteral("grey"), Qt::CaseInsensitive) == 0) {
        return Theme::Grey;
    }

    if (value.compare(QStringLiteral("mediumgray"), Qt::CaseInsensitive) == 0) {
        return Theme::MediumGray;
    }

    if (value.compare(QStringLiteral("darkgrey"), Qt::CaseInsensitive) == 0) {
        return Theme::DarkGrey;
    }

    if (value.compare(QStringLiteral("dark"), Qt::CaseInsensitive) == 0) {
        return Theme::Dark;
    }

    return Theme::Light;
}
//-------------------------------------------------------------------------------------------------
QString
themeToString(
    Theme theme
)
{
    switch (theme) {
    case Theme::LightGray:
        return QStringLiteral("lightgray");
    case Theme::Grey:
        return QStringLiteral("grey");
    case Theme::MediumGray:
        return QStringLiteral("mediumgray");
    case Theme::DarkGrey:
        return QStringLiteral("darkgrey");
    case Theme::Dark:
        return QStringLiteral("dark");
    case Theme::Light:
        return QStringLiteral("light");
    }

    return QStringLiteral("light");
}
//-------------------------------------------------------------------------------------------------
void
applyTheme(
    QApplication *application,
    Theme        theme
)
{
    if (application == nullptr) {
        return;
    }

    if (theme == Theme::Light) {
        application->setPalette(application->style()->standardPalette());
        application->setStyleSheet({});
        return;
    }

    QPalette palette;

    if (theme == Theme::LightGray) {
        palette.setColor(QPalette::Window, QColor(238, 238, 238));
        palette.setColor(QPalette::WindowText, QColor(30, 30, 30));
        palette.setColor(QPalette::Base, QColor(250, 250, 250));
        palette.setColor(QPalette::AlternateBase, QColor(242, 242, 242));
        palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
        palette.setColor(QPalette::ToolTipText, QColor(30, 30, 30));
        palette.setColor(QPalette::Text, QColor(30, 30, 30));
        palette.setColor(QPalette::Button, QColor(232, 232, 232));
        palette.setColor(QPalette::ButtonText, QColor(30, 30, 30));
        palette.setColor(QPalette::BrightText, QColor(180, 35, 35));
        palette.setColor(QPalette::Highlight, QColor(80, 135, 190));
        palette.setColor(QPalette::HighlightedText, Qt::white);
        palette.setColor(QPalette::Link, QColor(35, 95, 165));
        palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(135, 135, 135));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(135, 135, 135));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(135, 135, 135));

        application->setPalette(palette);
        application->setStyleSheet(QStringLiteral(
            "QToolTip { color: #1e1e1e; background-color: #ffffff; border: 1px solid #b8b8b8; }"
            "QTableView { gridline-color: #d0d0d0; }"
            "QHeaderView::section { background-color: #e8e8e8; color: #1e1e1e; border: 1px solid #c6c6c6; padding: 3px; }"
            "QMenuBar, QMenu { background-color: #eeeeee; color: #1e1e1e; }"
            "QMenu::item:selected, QMenuBar::item:selected { background-color: #5087be; color: #ffffff; }"
        ));
        return;
    }

    if (theme == Theme::MediumGray) {
        palette.setColor(QPalette::Window, QColor(145, 145, 150));
        palette.setColor(QPalette::WindowText, QColor(245, 245, 245));
        palette.setColor(QPalette::Base, QColor(112, 112, 118));
        palette.setColor(QPalette::AlternateBase, QColor(126, 126, 132));
        palette.setColor(QPalette::ToolTipBase, QColor(132, 132, 138));
        palette.setColor(QPalette::ToolTipText, QColor(245, 245, 245));
        palette.setColor(QPalette::Text, QColor(245, 245, 245));
        palette.setColor(QPalette::Button, QColor(156, 156, 162));
        palette.setColor(QPalette::ButtonText, QColor(245, 245, 245));
        palette.setColor(QPalette::BrightText, QColor(255, 90, 90));
        palette.setColor(QPalette::Highlight, QColor(82, 125, 170));
        palette.setColor(QPalette::HighlightedText, Qt::white);
        palette.setColor(QPalette::Link, QColor(70, 120, 190));
        palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(205, 205, 210));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(205, 205, 210));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(205, 205, 210));

        application->setPalette(palette);
        application->setStyleSheet(QStringLiteral(
            "QToolTip { color: #f5f5f5; background-color: #84848a; border: 1px solid #a8a8ae; }"
            "QTableView { gridline-color: #9a9aa0; }"
            "QHeaderView::section { background-color: #8c8c92; color: #f5f5f5; border: 1px solid #a8a8ae; padding: 3px; }"
            "QMenuBar, QMenu { background-color: #919196; color: #f5f5f5; }"
            "QMenu::item:selected, QMenuBar::item:selected { background-color: #527daa; color: #ffffff; }"
        ));
        return;
    }

    if (theme == Theme::DarkGrey) {
        palette.setColor(QPalette::Window, QColor(78, 78, 82));
        palette.setColor(QPalette::WindowText, QColor(235, 235, 235));
        palette.setColor(QPalette::Base, QColor(58, 58, 62));
        palette.setColor(QPalette::AlternateBase, QColor(68, 68, 72));
        palette.setColor(QPalette::ToolTipBase, QColor(70, 70, 74));
        palette.setColor(QPalette::ToolTipText, QColor(235, 235, 235));
        palette.setColor(QPalette::Text, QColor(235, 235, 235));
        palette.setColor(QPalette::Button, QColor(88, 88, 92));
        palette.setColor(QPalette::ButtonText, QColor(235, 235, 235));
        palette.setColor(QPalette::BrightText, QColor(255, 100, 100));
        palette.setColor(QPalette::Highlight, QColor(95, 130, 165));
        palette.setColor(QPalette::HighlightedText, Qt::white);
        palette.setColor(QPalette::Link, QColor(120, 180, 245));
        palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(150, 150, 154));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(150, 150, 154));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(150, 150, 154));

        application->setPalette(palette);
        application->setStyleSheet(QStringLiteral(
            "QToolTip { color: #ebebeb; background-color: #46464a; border: 1px solid #747478; }"
            "QTableView { gridline-color: #707074; }"
            "QHeaderView::section { background-color: #5a5a5e; color: #ebebeb; border: 1px solid #747478; padding: 3px; }"
            "QMenuBar, QMenu { background-color: #4e4e52; color: #ebebeb; }"
            "QMenu::item:selected, QMenuBar::item:selected { background-color: #5f82a5; color: #ffffff; }"
        ));
        return;
    }

    if (theme == Theme::Grey) {
        palette.setColor(QPalette::Window, QColor(214, 214, 214));
        palette.setColor(QPalette::WindowText, QColor(32, 32, 32));
        palette.setColor(QPalette::Base, QColor(236, 236, 236));
        palette.setColor(QPalette::AlternateBase, QColor(224, 224, 224));
        palette.setColor(QPalette::ToolTipBase, QColor(245, 245, 245));
        palette.setColor(QPalette::ToolTipText, QColor(32, 32, 32));
        palette.setColor(QPalette::Text, QColor(32, 32, 32));
        palette.setColor(QPalette::Button, QColor(202, 202, 202));
        palette.setColor(QPalette::ButtonText, QColor(32, 32, 32));
        palette.setColor(QPalette::BrightText, QColor(180, 40, 40));
        palette.setColor(QPalette::Highlight, QColor(105, 125, 150));
        palette.setColor(QPalette::HighlightedText, Qt::white);
        palette.setColor(QPalette::Link, QColor(40, 95, 150));
        palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(120, 120, 120));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(120, 120, 120));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(120, 120, 120));

        application->setPalette(palette);
        application->setStyleSheet(QStringLiteral(
            "QToolTip { color: #202020; background-color: #f5f5f5; border: 1px solid #9a9a9a; }"
            "QTableView { gridline-color: #b0b0b0; }"
            "QHeaderView::section { background-color: #c8c8c8; color: #202020; border: 1px solid #a8a8a8; padding: 3px; }"
            "QMenuBar, QMenu { background-color: #d6d6d6; color: #202020; }"
            "QMenu::item:selected, QMenuBar::item:selected { background-color: #697d96; color: #ffffff; }"
        ));
        return;
    }

    palette.setColor(QPalette::Window, QColor(45, 45, 48));
    palette.setColor(QPalette::WindowText, QColor(240, 240, 240));
    palette.setColor(QPalette::Base, QColor(30, 30, 30));
    palette.setColor(QPalette::AlternateBase, QColor(38, 38, 42));
    palette.setColor(QPalette::ToolTipBase, QColor(45, 45, 48));
    palette.setColor(QPalette::ToolTipText, QColor(240, 240, 240));
    palette.setColor(QPalette::Text, QColor(240, 240, 240));
    palette.setColor(QPalette::Button, QColor(55, 55, 60));
    palette.setColor(QPalette::ButtonText, QColor(240, 240, 240));
    palette.setColor(QPalette::BrightText, QColor(255, 90, 90));
    palette.setColor(QPalette::Highlight, QColor(45, 130, 210));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Link, QColor(95, 170, 255));
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(140, 140, 145));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(140, 140, 145));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(140, 140, 145));

    application->setPalette(palette);
    application->setStyleSheet(QStringLiteral(
        "QToolTip { color: #f0f0f0; background-color: #2d2d30; border: 1px solid #5a5a5f; }"
        "QTableView { gridline-color: #55555a; }"
        "QHeaderView::section { background-color: #3a3a3f; color: #f0f0f0; border: 1px solid #55555a; padding: 3px; }"
        "QMenuBar, QMenu { background-color: #2d2d30; color: #f0f0f0; }"
        "QMenu::item:selected, QMenuBar::item:selected { background-color: #2d82d2; }"
    ));
}
}
//-------------------------------------------------------------------------------------------------
