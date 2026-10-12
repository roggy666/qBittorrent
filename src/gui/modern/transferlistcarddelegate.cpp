/*
 * Bittorrent Client using Qt and libtorrent.
 * Modern UI (dark-blue) for the qBittorrent fork: card-style transfer list rows.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "transferlistcarddelegate.h"

#include <algorithm>

#include <QApplication>
#include <QFontMetrics>
#include <QIcon>
#include <QModelIndex>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

#include "base/global.h"
#include "gui/transferlistmodel.h"
#include "modernstyle.h"

using namespace ModernStyle;

namespace
{
    constexpr int CARD_MARGIN_X = 8;
    constexpr int CARD_MARGIN_Y = 4;
    constexpr int PAD = 16;
    constexpr int ICON_CIRCLE = 38;
    constexpr int STATS_WIDTH_WIDE = 330;
    constexpr int STATS_WIDTH_NARROW = 190;

    QString displayAt(const QModelIndex &index, const int column)
    {
        return index.siblingAtColumn(column).data(Qt::DisplayRole).toString();
    }

    QVariant underlyingAt(const QModelIndex &index, const int column)
    {
        return index.siblingAtColumn(column).data(TransferListModel::UnderlyingDataRole);
    }

    void drawPill(QPainter *painter, const QPointF &topLeft, const QString &text, const QColor &color, const QFont &font)
    {
        const QFontMetricsF fm {font};
        const qreal w = fm.horizontalAdvance(text) + 16;
        const qreal h = fm.height() + 5;
        const QRectF r {topLeft, QSizeF(w, h)};
        QColor bg = color; bg.setAlphaF(0.16);
        painter->setPen(Qt::NoPen);
        painter->setBrush(bg);
        painter->drawRoundedRect(r, h / 2, h / 2);
        painter->setPen(color);
        painter->setFont(font);
        painter->drawText(r, Qt::AlignCenter, text);
    }

    // returns the right edge x after drawing
    qreal drawStat(QPainter *painter, qreal x, const qreal y, const QString &label, const QString &value
        , const QColor &valueColor, const QFont &labelFont, const QFont &valueFont, const qreal minWidth)
    {
        const QFontMetricsF lfm {labelFont}, vfm {valueFont};
        const qreal w = std::max(minWidth, std::max(lfm.horizontalAdvance(label), vfm.horizontalAdvance(value)));
        painter->setFont(labelFont);
        painter->setPen(Colors::textDim);
        painter->drawText(QRectF(x, y, w, lfm.height()), Qt::AlignLeft | Qt::AlignVCenter, label);
        painter->setFont(valueFont);
        painter->setPen(valueColor);
        painter->drawText(QRectF(x, y + lfm.height() + 1, w, vfm.height()), Qt::AlignLeft | Qt::AlignVCenter, value);
        return x + w + 18;
    }
}

TransferListCardDelegate::TransferListCardDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

TransferListCardDelegate::StateInfo TransferListCardDelegate::stateInfo(const BitTorrent::TorrentState state, const QString &fallbackLabel)
{
    using BitTorrent::TorrentState;
    switch (state)
    {
    case TorrentState::Downloading:
    case TorrentState::ForcedDownloading:
        return {Colors::success, tr("Downloading")};
    case TorrentState::DownloadingMetadata:
    case TorrentState::ForcedDownloadingMetadata:
        return {Colors::success, tr("Metadata")};
    case TorrentState::StalledDownloading:
        return {Colors::warning, tr("Stalled")};
    case TorrentState::Uploading:
    case TorrentState::ForcedUploading:
        return {Colors::seeding, tr("Seeding")};
    case TorrentState::StalledUploading:
        return {Colors::seeding, tr("Seeding · idle")};
    case TorrentState::QueuedDownloading:
    case TorrentState::QueuedUploading:
        return {Colors::textMuted, tr("Queued")};
    case TorrentState::CheckingDownloading:
    case TorrentState::CheckingUploading:
    case TorrentState::CheckingResumeData:
        return {Colors::checking, tr("Checking")};
    case TorrentState::Moving:
        return {Colors::checking, tr("Moving")};
    case TorrentState::StoppedDownloading:
        return {Colors::paused, tr("Stopped"), false};
    case TorrentState::StoppedUploading:
        return {Colors::accent, tr("Completed"), false};
    case TorrentState::MissingFiles:
        return {Colors::danger, tr("Missing files"), false};
    case TorrentState::Error:
        return {Colors::danger, tr("Error"), false};
    default:
        return {Colors::textMuted, fallbackLabel};
    }
}

QSize TransferListCardDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const
{
    return {option.rect.width(), CARD_HEIGHT};
}

void TransferListCardDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);

    const bool selected = option.state.testFlag(QStyle::State_Selected);
    const bool hovered = option.state.testFlag(QStyle::State_MouseOver);
    const QRectF card = QRectF(option.rect).adjusted(CARD_MARGIN_X, CARD_MARGIN_Y, -CARD_MARGIN_X, -CARD_MARGIN_Y);

    // ---- card background ----
    QColor bg = selected ? Colors::accentSoft : (hovered ? Colors::surface2 : Colors::surface);
    painter->setPen(selected ? QPen(Colors::accent, 1) : QPen(Colors::border, 1));
    painter->setBrush(bg);
    painter->drawRoundedRect(card, RADIUS, RADIUS);

    // ---- data ----
    const QString name = displayAt(index, TransferListModel::TR_NAME);
    const QString statusText = displayAt(index, TransferListModel::TR_STATUS);
    const auto state = underlyingAt(index, TransferListModel::TR_STATUS).value<BitTorrent::TorrentState>();
    const StateInfo info = stateInfo(state, statusText);
    const qreal progress = std::clamp(underlyingAt(index, TransferListModel::TR_PROGRESS).toReal(), 0.0, 100.0);
    const QString size = displayAt(index, TransferListModel::TR_SIZE);
    const QString completed = displayAt(index, TransferListModel::TR_COMPLETED);
    const qint64 dlRate = underlyingAt(index, TransferListModel::TR_DLSPEED).toLongLong();
    const qint64 upRate = underlyingAt(index, TransferListModel::TR_UPSPEED).toLongLong();
    const QString dlSpeed = (dlRate > 0) ? displayAt(index, TransferListModel::TR_DLSPEED) : u"—"_s;
    const QString upSpeed = (upRate > 0) ? displayAt(index, TransferListModel::TR_UPSPEED) : u"—"_s;
    const QString eta = displayAt(index, TransferListModel::TR_ETA);
    const QString ratio = displayAt(index, TransferListModel::TR_RATIO);
    const QString seeds = displayAt(index, TransferListModel::TR_SEEDS);
    const QString peers = displayAt(index, TransferListModel::TR_PEERS);
    const QString category = displayAt(index, TransferListModel::TR_CATEGORY);
    const QIcon icon = index.siblingAtColumn(TransferListModel::TR_NAME).data(Qt::DecorationRole).value<QIcon>();

    // ---- fonts ----
    QFont nameFont = option.font; nameFont.setPointSizeF(option.font.pointSizeF() + 1); nameFont.setWeight(QFont::DemiBold);
    QFont smallFont = option.font; smallFont.setPointSizeF(option.font.pointSizeF() - 1);
    QFont pillFont = smallFont; pillFont.setWeight(QFont::DemiBold);
    QFont labelFont = option.font; labelFont.setPointSizeF(option.font.pointSizeF() - 1.5); labelFont.setCapitalization(QFont::AllUppercase); labelFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.6);
    QFont valueFont = option.font; valueFont.setWeight(QFont::DemiBold);
    QFont pctFont = option.font; pctFont.setPointSizeF(option.font.pointSizeF() + 3); pctFont.setWeight(QFont::Bold);

    const bool wide = card.width() >= 760;
    const bool narrow = card.width() < 560;
    const qreal statsWidth = narrow ? 0 : (wide ? STATS_WIDTH_WIDE : STATS_WIDTH_NARROW);

    // ---- status icon in a tinted circle ----
    const QRectF circle {card.left() + PAD, card.center().y() - ICON_CIRCLE / 2.0, qreal(ICON_CIRCLE), qreal(ICON_CIRCLE)};
    QColor circleBg = info.color; circleBg.setAlphaF(0.16);
    painter->setPen(Qt::NoPen);
    painter->setBrush(circleBg);
    painter->drawEllipse(circle);
    if (!icon.isNull())
    {
        const QRectF iconRect = circle.adjusted(9, 9, -9, -9);
        icon.paint(painter, iconRect.toRect(), Qt::AlignCenter, info.active ? QIcon::Normal : QIcon::Disabled);
    }

    // ---- left block: name, meta line, progress ----
    const qreal leftX = circle.right() + 14;
    const qreal rightLimit = card.right() - PAD - statsWidth;
    const qreal leftW = std::max(80.0, rightLimit - leftX);

    const QFontMetricsF nameFm {nameFont};
    const qreal nameY = card.top() + 12;
    painter->setFont(nameFont);
    painter->setPen(info.active ? Colors::text : Colors::textMuted);
    painter->drawText(QRectF(leftX, nameY, leftW, nameFm.height()), Qt::AlignLeft | Qt::AlignVCenter
        , nameFm.elidedText(name, Qt::ElideMiddle, leftW));

    // meta line: pill + size + category
    const qreal metaY = nameY + nameFm.height() + 5;
    qreal mx = leftX;
    {
        const QFontMetricsF pfm {pillFont};
        drawPill(painter, QPointF(mx, metaY), info.label, info.color, pillFont);
        mx += pfm.horizontalAdvance(info.label) + 16 + 10;
        if (!category.isEmpty())
        {
            drawPill(painter, QPointF(mx, metaY), category, Colors::textMuted, pillFont);
            mx += pfm.horizontalAdvance(category) + 16 + 10;
        }
        const QFontMetricsF sfm {smallFont};
        QString meta = (progress < 100.0 && !completed.isEmpty()) ? u"%1 / %2"_s.arg(completed, size) : size;
        painter->setFont(smallFont);
        painter->setPen(Colors::textMuted);
        painter->drawText(QRectF(mx, metaY, std::max(0.0, rightLimit - mx), pfm.height() + 5), Qt::AlignLeft | Qt::AlignVCenter
            , sfm.elidedText(meta, Qt::ElideRight, std::max(0.0, rightLimit - mx)));
    }

    // progress track
    {
        const qreal barH = 6;
        const qreal barY = card.bottom() - 14 - barH;
        const QRectF track {leftX, barY, leftW, barH};
        painter->setPen(Qt::NoPen);
        painter->setBrush(Colors::track);
        painter->drawRoundedRect(track, barH / 2, barH / 2);
        if (progress > 0)
        {
            QRectF fill = track; fill.setWidth(std::max(barH, track.width() * progress / 100.0));
            QColor c = info.active ? info.color : Colors::paused;
            if (state == BitTorrent::TorrentState::StoppedUploading) c = Colors::accent;
            painter->setBrush(c);
            painter->drawRoundedRect(fill, barH / 2, barH / 2);
        }
    }

    // ---- right block: stats ----
    if (!narrow)
    {
        const qreal sx0 = card.right() - PAD - statsWidth;
        const qreal rowY = card.top() + 14;
        qreal sx = sx0;
        // percent (big)
        painter->setFont(pctFont);
        painter->setPen(info.active ? Colors::text : Colors::textMuted);
        const QString pct = (progress >= 100.0) ? u"100%"_s : u"%1%"_s.arg(QString::number(progress, 'f', 1));
        const QFontMetricsF pfm {pctFont};
        const qreal pctW = pfm.horizontalAdvance(u"100.0%"_s);
        painter->drawText(QRectF(card.right() - PAD - pctW, card.bottom() - 14 - pfm.height() + 2, pctW, pfm.height()), Qt::AlignRight | Qt::AlignVCenter, pct);

        sx = drawStat(painter, sx, rowY, tr("Down"), dlSpeed, (dlRate > 0) ? Colors::success : Colors::textDim, labelFont, valueFont, 74);
        sx = drawStat(painter, sx, rowY, tr("Up"), upSpeed, (upRate > 0) ? Colors::accentHover : Colors::textDim, labelFont, valueFont, 74);
        if (wide)
        {
            sx = drawStat(painter, sx, rowY, tr("ETA"), eta, Colors::text, labelFont, valueFont, 56);
            sx = drawStat(painter, sx, rowY, tr("Ratio"), ratio, Colors::text, labelFont, valueFont, 40);
        }
        // seeds/peers line under stats (left part of the stats block)
        painter->setFont(smallFont);
        painter->setPen(Colors::textDim);
        const QFontMetricsF sfm {smallFont};
        painter->drawText(QRectF(sx0, card.bottom() - 14 - sfm.height(), statsWidth - pctW - 8, sfm.height()), Qt::AlignLeft | Qt::AlignVCenter
            , tr("Seeds %1  ·  Peers %2").arg(seeds, peers));
    }

    painter->restore();
}
