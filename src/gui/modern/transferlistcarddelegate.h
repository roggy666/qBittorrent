/*
 * Bittorrent Client using Qt and libtorrent.
 * Modern UI (dark-blue) for the qBittorrent fork: card-style transfer list rows.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#pragma once

#include <QStyledItemDelegate>

#include "base/bittorrent/torrent.h"

class TransferListCardDelegate final : public QStyledItemDelegate
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TransferListCardDelegate)

public:
    explicit TransferListCardDelegate(QObject *parent = nullptr);

    static constexpr int CARD_HEIGHT = 86;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    struct StateInfo
    {
        QColor color;
        QString label;
        bool active = true;
    };
    static StateInfo stateInfo(BitTorrent::TorrentState state, const QString &fallbackLabel);
};
