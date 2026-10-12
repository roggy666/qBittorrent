/*
 * Bittorrent Client using Qt and libtorrent.
 * Modern UI (dark-blue) for the qBittorrent fork.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "modernstyle.h"

#include <QApplication>
#include <QFont>
#include <QFontDatabase>
#include <QPalette>
#include <QStyleHints>
#include <QWidget>

#ifdef Q_OS_WIN
#include <QtCore/qt_windows.h>
#include <dwmapi.h>
#endif

using namespace ModernStyle;

namespace
{
    QString hex(const QColor &c)
    {
        return c.name(QColor::HexRgb);
    }

    QString rgba(const QColor &c, const double alpha)
    {
        return u"rgba(%1,%2,%3,%4)"_s.arg(c.red()).arg(c.green()).arg(c.blue()).arg(alpha);
    }
}

QPalette ModernStyle::palette()
{
    QPalette p;
    p.setColor(QPalette::Window, Colors::bg);
    p.setColor(QPalette::WindowText, Colors::text);
    p.setColor(QPalette::Base, Colors::surface);
    p.setColor(QPalette::AlternateBase, Colors::surface2);
    p.setColor(QPalette::Text, Colors::text);
    p.setColor(QPalette::PlaceholderText, Colors::textDim);
    p.setColor(QPalette::ToolTipBase, Colors::surface2);
    p.setColor(QPalette::ToolTipText, Colors::text);
    p.setColor(QPalette::BrightText, Qt::white);
    p.setColor(QPalette::Button, Colors::surface2);
    p.setColor(QPalette::ButtonText, Colors::text);
    p.setColor(QPalette::Highlight, Colors::accent);
    p.setColor(QPalette::HighlightedText, Qt::white);
    p.setColor(QPalette::Link, Colors::accentHover);
    p.setColor(QPalette::LinkVisited, Colors::accent);
    p.setColor(QPalette::Light, Colors::surface3);
    p.setColor(QPalette::Midlight, Colors::surface2);
    p.setColor(QPalette::Mid, Colors::border);
    p.setColor(QPalette::Dark, Colors::bg);
    p.setColor(QPalette::Shadow, Qt::black);
    for (const auto role : {QPalette::WindowText, QPalette::Text, QPalette::ButtonText, QPalette::HighlightedText})
        p.setColor(QPalette::Disabled, role, Colors::textDim);
    p.setColor(QPalette::Disabled, QPalette::Button, Colors::surface);
    p.setColor(QPalette::Disabled, QPalette::Base, Colors::surface);
    p.setColor(QPalette::Disabled, QPalette::Highlight, Colors::surface3);
    return p;
}

QString ModernStyle::styleSheet()
{
    const QString bg = hex(Colors::bg), surface = hex(Colors::surface), surface2 = hex(Colors::surface2)
        , surface3 = hex(Colors::surface3), border = hex(Colors::border), text = hex(Colors::text)
        , muted = hex(Colors::textMuted), dim = hex(Colors::textDim), accent = hex(Colors::accent)
        , accentHover = hex(Colors::accentHover), accentSoft = hex(Colors::accentSoft), track = hex(Colors::track);
    const QString r = QString::number(RADIUS), rs = QString::number(RADIUS_SM);

    QString qss = uR"(
/* ---------- base ---------- */
QWidget { background: transparent; color: %TEXT%; selection-background-color: %ACCENT%; selection-color: #ffffff; outline: none; }
QMainWindow, QDialog, QDockWidget { background: %BG%; }
QMainWindow::separator { background: %BG%; width: 6px; height: 6px; }
QToolTip { background: %SURFACE2%; color: %TEXT%; border: 1px solid %SURFACE3%; border-radius: %RS%px; padding: 6px 8px; }
QFrame[frameShape="4"], QFrame[frameShape="5"] { color: %BORDER%; }

/* ---------- menus ---------- */
QMenuBar { background: %BG%; padding: 2px 6px; border: none; }
QMenuBar::item { padding: 5px 10px; border-radius: %RS%px; color: %MUTED%; }
QMenuBar::item:selected { background: %SURFACE2%; color: %TEXT%; }
QMenu { background: %SURFACE%; border: 1px solid %SURFACE3%; border-radius: %RS%px; padding: 6px; }
QMenu::item { padding: 7px 28px 7px 12px; border-radius: 6px; }
QMenu::item:selected { background: %ACCENTSOFT%; color: #ffffff; }
QMenu::item:disabled { color: %DIM%; }
QMenu::separator { height: 1px; background: %SURFACE3%; margin: 6px 8px; }
QMenu::icon { padding-left: 8px; }
QMenu::indicator { width: 14px; height: 14px; left: 8px; }

/* ---------- toolbar ---------- */
QToolBar { background: %BG%; border: none; padding: 6px 10px; spacing: 4px; }
QToolBar::separator { width: 1px; background: %SURFACE3%; margin: 8px 6px; }
QToolButton { background: transparent; border: 1px solid transparent; border-radius: %RS%px; padding: 7px 10px; color: %TEXT%; }
QToolButton:hover { background: %SURFACE2%; }
QToolButton:pressed, QToolButton:checked { background: %SURFACE3%; }
QToolButton:disabled { color: %DIM%; }
QToolButton::menu-indicator { image: none; width: 0; }
QToolButton#primaryAction { background: %ACCENT%; color: #ffffff; padding: 7px 14px; font-weight: 600; }
QToolButton#primaryAction:hover { background: %ACCENTHOVER%; }
QToolButton#primaryAction:pressed { background: %ACCENTSOFT%; }

/* ---------- inputs ---------- */
QLineEdit, QPlainTextEdit, QTextEdit, QSpinBox, QDoubleSpinBox, QComboBox, QDateTimeEdit, QTimeEdit, QDateEdit {
    background: %SURFACE2%; border: 1px solid %BORDER%; border-radius: %RS%px; padding: 6px 10px; min-height: 18px; selection-background-color: %ACCENT%; }
QLineEdit:focus, QPlainTextEdit:focus, QTextEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus { border: 1px solid %ACCENT%; }
QLineEdit:disabled, QComboBox:disabled, QSpinBox:disabled { color: %DIM%; background: %SURFACE%; }
QLineEdit#searchField { padding-left: 12px; min-width: 240px; }
QComboBox { padding-right: 26px; }
QComboBox::drop-down { border: none; width: 24px; subcontrol-origin: padding; subcontrol-position: center right; }
QComboBox::down-arrow { image: none; border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 5px solid %MUTED%; width: 0; height: 0; margin-right: 10px; }
QComboBox QAbstractItemView { background: %SURFACE%; border: 1px solid %SURFACE3%; border-radius: %RS%px; padding: 4px; selection-background-color: %ACCENTSOFT%; outline: none; }
QComboBox QAbstractItemView::item { padding: 6px 8px; border-radius: 6px; min-height: 22px; }
QSpinBox::up-button, QDoubleSpinBox::up-button, QSpinBox::down-button, QDoubleSpinBox::down-button { width: 18px; border: none; background: transparent; }
QSpinBox::up-arrow, QDoubleSpinBox::up-arrow { border-left: 4px solid transparent; border-right: 4px solid transparent; border-bottom: 5px solid %MUTED%; width: 0; height: 0; }
QSpinBox::down-arrow, QDoubleSpinBox::down-arrow { border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 5px solid %MUTED%; width: 0; height: 0; }

/* ---------- buttons ---------- */
QPushButton { background: %SURFACE2%; border: 1px solid %BORDER%; border-radius: %RS%px; padding: 7px 16px; min-height: 18px; }
QPushButton:hover { background: %SURFACE3%; border-color: %SURFACE3%; }
QPushButton:pressed { background: %ACCENTSOFT%; }
QPushButton:checked { background: %ACCENTSOFT%; border-color: %ACCENT%; }
QPushButton:default { background: %ACCENT%; border-color: %ACCENT%; color: #ffffff; font-weight: 600; }
QPushButton:default:hover { background: %ACCENTHOVER%; }
QPushButton:disabled { color: %DIM%; background: %SURFACE%; border-color: %SURFACE%; }
QPushButton:flat { background: transparent; border: none; }
QPushButton:flat:hover { background: %SURFACE2%; }
QPushButton#propTab { background: transparent; border: 1px solid transparent; border-radius: 999px; padding: 5px 14px; color: %MUTED%; }
QPushButton#propTab:hover { background: %SURFACE2%; color: %TEXT%; }
QPushButton#propTab:pressed, QPushButton#propTab:checked { background: %ACCENTSOFT%; color: #ffffff; border-color: %ACCENT%; }

/* ---------- check / radio ---------- */
QCheckBox, QRadioButton { spacing: 8px; padding: 2px 0; }
QCheckBox::indicator, QRadioButton::indicator, QGroupBox::indicator, QTreeView::indicator, QListView::indicator { width: 16px; height: 16px; border: 1px solid %SURFACE3%; background: %SURFACE2%; }
QCheckBox::indicator, QGroupBox::indicator, QTreeView::indicator, QListView::indicator { border-radius: 4px; }
QRadioButton::indicator { border-radius: 8px; }
QCheckBox::indicator:hover, QRadioButton::indicator:hover { border-color: %ACCENT%; }
QCheckBox::indicator:checked, QGroupBox::indicator:checked, QTreeView::indicator:checked, QListView::indicator:checked { background: %ACCENT%; border-color: %ACCENT%; image: url(:/modern/check.svg); }
QCheckBox::indicator:indeterminate, QTreeView::indicator:indeterminate { background: %ACCENTSOFT%; border-color: %ACCENT%; image: url(:/modern/minus.svg); }
QRadioButton::indicator:checked { background: %ACCENT%; border: 4px solid %SURFACE2%; outline: none; }
QCheckBox::indicator:disabled, QRadioButton::indicator:disabled { background: %SURFACE%; border-color: %SURFACE%; }

/* ---------- group box ---------- */
QGroupBox { border: 1px solid %BORDER%; border-radius: %R%px; margin-top: 14px; padding: 12px 8px 6px 8px; }
QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 12px; padding: 0 6px; color: %MUTED%; font-weight: 600; }

/* ---------- tabs ---------- */
QTabWidget::pane { border: 1px solid %BORDER%; border-radius: %R%px; top: -1px; background: %SURFACE%; }
QTabWidget#mainTabs::pane { border: none; background: transparent; }
QTabBar { background: transparent; }
QTabBar::tab { background: transparent; color: %MUTED%; padding: 8px 16px; margin-right: 2px; border-top-left-radius: %RS%px; border-top-right-radius: %RS%px; border: 1px solid transparent; border-bottom: none; }
QTabBar::tab:hover { color: %TEXT%; background: %SURFACE2%; }
QTabBar::tab:selected { color: %TEXT%; background: %SURFACE%; border-color: %BORDER%; }
QTabBar::close-button { image: url(:/modern/close.svg); subcontrol-position: right; }
QTabBar::scroller { width: 24px; }

/* ---------- item views ---------- */
QTreeView, QListView, QTableView, QTreeWidget, QListWidget, QTableWidget { background: %SURFACE%; alternate-background-color: %SURFACE%; border: 1px solid %BORDER%; border-radius: %R%px; padding: 4px; show-decoration-selected: 1; }
QTreeView::item, QListView::item, QTableView::item { padding: 5px 6px; border-radius: 6px; border: none; }
QTreeView::item:hover, QListView::item:hover, QTableView::item:hover { background: %SURFACE2%; }
QTreeView::item:selected, QListView::item:selected, QTableView::item:selected { background: %ACCENTSOFT%; color: #ffffff; }
QTreeView::item:selected:!active, QListView::item:selected:!active { background: %SURFACE3%; color: %TEXT%; }
QTreeView::branch { background: transparent; }
QTreeView::branch:has-children:!has-siblings:closed, QTreeView::branch:closed:has-children:has-siblings { image: url(:/modern/chevron-right.svg); }
QTreeView::branch:open:has-children:!has-siblings, QTreeView::branch:open:has-children:has-siblings { image: url(:/modern/chevron-down.svg); }
QTreeView#transferList { padding: 0; border: none; background: transparent; }
QTreeView#transferList::item { padding: 0; border-radius: 0; }
QTreeView#transferList::item:hover, QTreeView#transferList::item:selected { background: transparent; color: %TEXT%; }
QHeaderView { background: transparent; border: none; }
QHeaderView::section { background: %SURFACE%; color: %MUTED%; padding: 7px 8px; border: none; border-bottom: 1px solid %BORDER%; border-right: 1px solid %BORDER%; font-weight: 600; }
QHeaderView::section:hover { background: %SURFACE2%; color: %TEXT%; }
QHeaderView::section:first { border-top-left-radius: %RS%px; }
QHeaderView::section:last { border-right: none; border-top-right-radius: %RS%px; }
QHeaderView::down-arrow { image: url(:/modern/chevron-down.svg); width: 12px; height: 12px; subcontrol-position: center right; right: 6px; }
QHeaderView::up-arrow { image: url(:/modern/chevron-up.svg); width: 12px; height: 12px; subcontrol-position: center right; right: 6px; }
QTableView QTableCornerButton::section { background: %SURFACE%; border: none; }

/* ---------- sidebar ---------- */
QWidget#sidebar { background: %SURFACE%; border: 1px solid %BORDER%; border-radius: %R%px; }
QWidget#sidebar QScrollArea, QWidget#sidebar QScrollArea > QWidget > QWidget { background: transparent; }
QWidget#sidebar QCheckBox { color: %DIM%; font-size: 8pt; font-weight: 700; letter-spacing: 1px; padding: 10px 10px 2px 10px; }
QWidget#sidebar QCheckBox::indicator { width: 10px; height: 10px; border: none; background: transparent; }
QWidget#sidebar QListWidget, QWidget#sidebar QTreeView { background: transparent; border: none; border-radius: 0; padding: 0 6px; }
QWidget#sidebar QListWidget::item, QWidget#sidebar QTreeView::item { padding: 7px 10px; border-radius: 8px; margin: 1px 0; color: %MUTED%; }
QWidget#sidebar QListWidget::item:hover, QWidget#sidebar QTreeView::item:hover { background: %SURFACE2%; color: %TEXT%; }
QWidget#sidebar QListWidget::item:selected, QWidget#sidebar QTreeView::item:selected { background: %ACCENTSOFT%; color: #ffffff; }

/* ---------- scrollbars ---------- */
QScrollBar:vertical { background: transparent; width: 12px; margin: 2px; }
QScrollBar::handle:vertical { background: %SURFACE3%; border-radius: 4px; min-height: 30px; }
QScrollBar::handle:vertical:hover { background: %DIM%; }
QScrollBar:horizontal { background: transparent; height: 12px; margin: 2px; }
QScrollBar::handle:horizontal { background: %SURFACE3%; border-radius: 4px; min-width: 30px; }
QScrollBar::handle:horizontal:hover { background: %DIM%; }
QScrollBar::add-line, QScrollBar::sub-line, QScrollBar::add-page, QScrollBar::sub-page { background: none; border: none; width: 0; height: 0; }

/* ---------- splitter ---------- */
QSplitter::handle { background: transparent; }
QSplitter::handle:horizontal { width: 8px; }
QSplitter::handle:vertical { height: 8px; }
QSplitter::handle:hover { background: %SURFACE2%; border-radius: 3px; }

/* ---------- progress ---------- */
QProgressBar { background: %TRACK%; border: none; border-radius: 4px; height: 8px; text-align: center; color: transparent; }
QProgressBar::chunk { background: %ACCENT%; border-radius: 4px; }

/* ---------- status bar ---------- */
QStatusBar { background: %BG%; border-top: 1px solid %BORDER%; color: %MUTED%; padding: 2px 8px; }
QStatusBar QLabel { color: %MUTED%; padding: 0 4px; }
QStatusBar QPushButton { background: transparent; border: none; padding: 3px 8px; border-radius: %RS%px; color: %MUTED%; }
QStatusBar QPushButton:hover { background: %SURFACE2%; color: %TEXT%; }
QStatusBar QFrame[frameShape="5"] { color: %SURFACE3%; }
QSizeGrip { width: 0; height: 0; }

/* ---------- misc ---------- */
QSlider::groove:horizontal { height: 6px; background: %TRACK%; border-radius: 3px; }
QSlider::handle:horizontal { background: %ACCENT%; width: 16px; height: 16px; margin: -5px 0; border-radius: 8px; }
QSlider::sub-page:horizontal { background: %ACCENT%; border-radius: 3px; }
QLabel#dimLabel { color: %DIM%; }
QLabel#mutedLabel { color: %MUTED%; }
QDialogButtonBox QPushButton { min-width: 84px; }
QMessageBox { background: %BG%; }
QAbstractScrollArea::corner { background: transparent; }
QTextBrowser, QTextEdit, QPlainTextEdit { background: %SURFACE%; border: 1px solid %BORDER%; border-radius: %R%px; }
QToolBox::tab { background: %SURFACE2%; border-radius: %RS%px; padding: 6px; }
QDockWidget::title { background: %SURFACE%; padding: 6px; border-radius: %RS%px; }
QCalendarWidget QWidget { background: %SURFACE%; }
)"_s;

    qss.replace(u"%BG%"_s, bg).replace(u"%SURFACE2%"_s, surface2).replace(u"%SURFACE3%"_s, surface3)
       .replace(u"%SURFACE%"_s, surface).replace(u"%BORDER%"_s, border).replace(u"%TEXT%"_s, text)
       .replace(u"%MUTED%"_s, muted).replace(u"%DIM%"_s, dim).replace(u"%ACCENTHOVER%"_s, accentHover)
       .replace(u"%ACCENTSOFT%"_s, accentSoft).replace(u"%ACCENT%"_s, accent).replace(u"%TRACK%"_s, track)
       .replace(u"%RS%"_s, rs).replace(u"%R%"_s, r);
    return qss;
}

void ModernStyle::apply()
{
    QApplication::setStyle(u"Fusion"_s);
    qApp->styleHints()->setColorScheme(Qt::ColorScheme::Dark);
    QApplication::setPalette(palette());

    QFont font = QApplication::font();
    const QStringList families = QFontDatabase::families();
    for (const QString &candidate : {u"Segoe UI Variable Text"_s, u"Segoe UI"_s, u"Inter"_s})
    {
        if (families.contains(candidate))
        {
            font.setFamily(candidate);
            break;
        }
    }
    font.setPointSizeF(9.5);
    QApplication::setFont(font);

    qApp->setStyleSheet(styleSheet());
}

void ModernStyle::applyWindowFrame([[maybe_unused]] QWidget *window)
{
#ifdef Q_OS_WIN
    if (!window)
        return;
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());
    const BOOL dark = TRUE;
    ::DwmSetWindowAttribute(hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &dark, sizeof(dark));
    const COLORREF caption = RGB(Colors::bg.red(), Colors::bg.green(), Colors::bg.blue());
    ::DwmSetWindowAttribute(hwnd, 35 /* DWMWA_CAPTION_COLOR */, &caption, sizeof(caption));
    const COLORREF captionText = RGB(Colors::text.red(), Colors::text.green(), Colors::text.blue());
    ::DwmSetWindowAttribute(hwnd, 36 /* DWMWA_TEXT_COLOR */, &captionText, sizeof(captionText));
    const COLORREF borderColor = RGB(Colors::border.red(), Colors::border.green(), Colors::border.blue());
    ::DwmSetWindowAttribute(hwnd, 34 /* DWMWA_BORDER_COLOR */, &borderColor, sizeof(borderColor));
#endif
}
