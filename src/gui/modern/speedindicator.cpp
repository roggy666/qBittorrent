/*
 * Bittorrent Client using Qt and libtorrent.
 * Modern UI (dark-blue) for the qBittorrent fork: toolbar speed indicator with sparklines.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "speedindicator.h"

#include <algorithm>

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>

#include "base/bittorrent/session.h"
#include "base/bittorrent/sessionstatus.h"
#include "base/global.h"
#include "base/utils/misc.h"
#include "modernstyle.h"

using namespace ModernStyle;

SpeedIndicator::SpeedIndicator(QWidget *parent)
    : QWidget(parent)
    , m_timer {new QTimer(this)}
{
    setCursor(Qt::PointingHandCursor);
    setToolTip(tr("Download / upload speed. Click to set global speed limits."));
    setAttribute(Qt::WA_Hover);
    setFixedSize(sizeHint());
    m_dl.reserve(HISTORY);
    m_up.reserve(HISTORY);
    connect(m_timer, &QTimer::timeout, this, &SpeedIndicator::sample);
    m_timer->start(1000);
    sample();
}

QSize SpeedIndicator::sizeHint() const
{
    return {280, 40};
}

void SpeedIndicator::sample()
{
    const BitTorrent::SessionStatus &status = BitTorrent::Session::instance()->status();
    m_dlNow = status.payloadDownloadRate;
    m_upNow = status.payloadUploadRate;
    m_dl.append(m_dlNow);
    m_up.append(m_upNow);
    while (m_dl.size() > HISTORY) m_dl.removeFirst();
    while (m_up.size() > HISTORY) m_up.removeFirst();
    update();
}

void SpeedIndicator::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit clicked();
    QWidget::mouseReleaseEvent(event);
}

void SpeedIndicator::paintEvent(QPaintEvent *)
{
    QPainter p {this};
    p.setRenderHint(QPainter::Antialiasing, true);
    const QRectF r = rect();

    // pill background
    p.setPen(QPen(Colors::border, 1));
    p.setBrush(underMouse() ? Colors::surface2 : Colors::surface);
    p.drawRoundedRect(r.adjusted(0.5, 0.5, -0.5, -0.5), RADIUS, RADIUS);

    // sparkline area (left part)
    const QRectF graph = QRectF(r.left() + 10, r.top() + 7, 96, r.height() - 14);
    const auto drawSpark = [&](const QList<qint64> &data, const QColor &color)
    {
        if (data.size() < 2) return;
        const qint64 maxV = std::max<qint64>(1, *std::max_element(data.cbegin(), data.cend()));
        QPainterPath path;
        const qreal stepX = graph.width() / (HISTORY - 1);
        const int offset = HISTORY - data.size();
        for (int i = 0; i < data.size(); ++i)
        {
            const qreal x = graph.left() + (offset + i) * stepX;
            const qreal y = graph.bottom() - (graph.height() * data[i] / qreal(maxV));
            if (i == 0) path.moveTo(x, y); else path.lineTo(x, y);
        }
        QPainterPath fill = path;
        fill.lineTo(graph.right(), graph.bottom());
        fill.lineTo(graph.left() + offset * stepX, graph.bottom());
        fill.closeSubpath();
        QColor fc = color; fc.setAlphaF(0.18);
        p.setPen(Qt::NoPen); p.setBrush(fc); p.drawPath(fill);
        p.setPen(QPen(color, 1.5)); p.setBrush(Qt::NoBrush); p.drawPath(path);
    };
    drawSpark(m_up, Colors::accentHover);
    drawSpark(m_dl, Colors::success);

    // texts
    QFont f = font(); f.setWeight(QFont::DemiBold);
    QFont small = font(); small.setPointSizeF(font().pointSizeF() - 1.5);
    const qreal tx = graph.right() + 14;
    const qreal colW = (r.right() - 10 - tx) / 2;
    const auto drawValue = [&](const qreal x, const QString &arrow, const QString &label, const qint64 v, const QColor &c)
    {
        p.setFont(small); p.setPen(Colors::textDim);
        p.drawText(QRectF(x, r.top() + 5, colW, 13), Qt::AlignLeft | Qt::AlignVCenter, label);
        p.setFont(f); p.setPen(v > 0 ? c : Colors::textMuted);
        p.drawText(QRectF(x, r.top() + 17, colW, 16), Qt::AlignLeft | Qt::AlignVCenter
            , arrow + u" "_s + Utils::Misc::friendlyUnit(v, true));
    };
    drawValue(tx, u"↓"_s, tr("Download"), m_dlNow, Colors::success);
    drawValue(tx + colW, u"↑"_s, tr("Upload"), m_upNow, Colors::accentHover);
}
