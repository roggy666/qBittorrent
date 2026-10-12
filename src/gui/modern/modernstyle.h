/*
 * Bittorrent Client using Qt and libtorrent.
 * Modern UI (dark-blue) for the qBittorrent fork.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#pragma once

#include <QColor>
#include <QString>

#include "base/global.h"

class QPalette;
class QWidget;

namespace ModernStyle
{
    // Design tokens (dark-blue)
    namespace Colors
    {
        inline const QColor bg          {u"#0b1220"_s};   // window background
        inline const QColor surface     {u"#111a2e"_s};   // panels, cards
        inline const QColor surface2    {u"#1a2540"_s};   // hover / inputs
        inline const QColor surface3    {u"#243253"_s};   // pressed / borders strong
        inline const QColor border      {u"#223050"_s};
        inline const QColor text        {u"#e5ecf8"_s};
        inline const QColor textMuted   {u"#8ea0bf"_s};
        inline const QColor textDim     {u"#5f7196"_s};
        inline const QColor accent      {u"#3b82f6"_s};
        inline const QColor accentHover {u"#60a5fa"_s};
        inline const QColor accentSoft  {u"#1d3a6b"_s};   // selection bg
        inline const QColor success     {u"#22c55e"_s};
        inline const QColor seeding     {u"#14b8a6"_s};
        inline const QColor warning     {u"#f59e0b"_s};
        inline const QColor danger      {u"#ef4444"_s};
        inline const QColor paused      {u"#64748b"_s};
        inline const QColor checking    {u"#a78bfa"_s};
        inline const QColor track       {u"#1e2a44"_s};   // progress track
    }

    constexpr int RADIUS = 10;
    constexpr int RADIUS_SM = 7;

    QPalette palette();
    QString styleSheet();
    // Applies palette, font, style and stylesheet to the application
    void apply();
    // Dark title bar on Windows 11 (no-op elsewhere)
    void applyWindowFrame(QWidget *window);
}
