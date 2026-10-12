/*
 * Bittorrent Client using Qt and libtorrent.
 * Modern UI (dark-blue) for the qBittorrent fork: toolbar speed indicator with sparklines.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#pragma once

#include <QList>
#include <QWidget>

class QTimer;

class SpeedIndicator final : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(SpeedIndicator)

public:
    explicit SpeedIndicator(QWidget *parent = nullptr);

    QSize sizeHint() const override;

signals:
    void clicked();

private:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void sample();

    static constexpr int HISTORY = 48;
    QList<qint64> m_dl;
    QList<qint64> m_up;
    qint64 m_dlNow = 0;
    qint64 m_upNow = 0;
    QTimer *m_timer = nullptr;
};
