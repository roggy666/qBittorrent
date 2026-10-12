/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2025  Vladimir Golovnev <glassez@yandex.ru>
 * Copyright (C) 2006  Christophe Dumez <chris@qbittorrent.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If you
 * modify file(s), you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version.
 */

#include "transferlistdelegate.h"

#include <algorithm>

#include <QApplication>
#include <QFontMetricsF>
#include <QModelIndex>
#include <QPainter>
#include <QStyle>

#include "base/preferences.h"
#include "modern/modernstyle.h"
#include "transferlistmodel.h"

TransferListDelegate::TransferListDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QSize TransferListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Reimplementing sizeHint() because the 'name' column contains text+icon.
    // When that WHOLE column goes out of view(eg user scrolls horizontally)
    // the rows shrink if the text's height is smaller than the icon's height.
    // This happens because icon from the 'name' column is no longer drawn.

    if (m_nameColHeight == -1)
    {
        const QModelIndex nameColumn = index.sibling(index.row(), TransferListModel::TR_NAME);
        m_nameColHeight = QStyledItemDelegate::sizeHint(option, nameColumn).height();
    }

    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(std::max(m_nameColHeight, size.height()));
    return size;
}

void TransferListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() != TransferListModel::TR_PROGRESS)
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    using namespace BitTorrent;

    QStyleOptionViewItem opt {option};
    initStyleOption(&opt, index);
    opt.text.clear();
    const QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

    const qreal progress = std::clamp(index.data(TransferListModel::UnderlyingDataRole).toReal(), 0.0, 100.0);
    const QModelIndex statusIndex = index.siblingAtColumn(TransferListModel::TR_STATUS);
    const auto torrentState = statusIndex.data(TransferListModel::UnderlyingDataRole).value<TorrentState>();
    QColor fill = statusIndex.data(Qt::ForegroundRole).value<QColor>();
    if (!fill.isValid())
        fill = ModernStyle::Colors::accent;
    switch (torrentState)
    {
    case TorrentState::StoppedDownloading:
    case TorrentState::Unknown:
        fill = ModernStyle::Colors::paused;
        break;
    case TorrentState::StoppedUploading:
        fill = ModernStyle::Colors::accent;
        break;
    case TorrentState::Error:
    case TorrentState::MissingFiles:
        fill = ModernStyle::Colors::danger;
        break;
    default:
        break;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    const QRectF cell = QRectF(opt.rect).adjusted(8, 0, -8, 0);
    QFont pctFont = opt.font;
    pctFont.setPointSizeF(opt.font.pointSizeF() - 0.5);
    pctFont.setWeight(QFont::DemiBold);
    const QFontMetricsF fm {pctFont};
    const qreal textW = fm.horizontalAdvance(u"100.0%"_s) + 4;
    const qreal barH = 7;
    const QRectF track {cell.left(), cell.center().y() - barH / 2, std::max(20.0, cell.width() - textW - 8), barH};
    painter->setPen(Qt::NoPen);
    painter->setBrush(ModernStyle::Colors::track);
    painter->drawRoundedRect(track, barH / 2, barH / 2);
    if (progress > 0)
    {
        QRectF done = track;
        done.setWidth(std::max(barH, track.width() * progress / 100.0));
        painter->setBrush(fill);
        painter->drawRoundedRect(done, barH / 2, barH / 2);
    }
    painter->setFont(pctFont);
    painter->setPen(opt.state.testFlag(QStyle::State_Selected) ? QColor(Qt::white) : ModernStyle::Colors::textMuted);
    const QString pct = (progress >= 100.0) ? u"100%"_s : u"%1%"_s.arg(QString::number(progress, 'f', 1));
    painter->drawText(QRectF(track.right() + 8, cell.top(), textW, cell.height()), Qt::AlignLeft | Qt::AlignVCenter, pct);
    painter->restore();
}
