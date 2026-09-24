#include "ui/TimelineWidget.hpp"

#include <QMouseEvent>
#include <QPainter>

#include <algorithm>

namespace pmp::ui {

TimelineWidget::TimelineWidget(QWidget* parent)
    : QWidget(parent) {
    setMinimumHeight(120);
    setMouseTracking(true);
}

void TimelineWidget::sync(const core::WorkbenchSnapshot& snapshot) {
    timeline_ = snapshot.timeline;
    update();
}

void TimelineWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor("#14181e"));
    const QRectF track = rect().adjusted(20, 34, -20, -24);
    painter.setPen(QPen(QColor("#303b4a"), 1));
    painter.drawRect(track);
    painter.setPen(QColor("#718096"));
    for (int second = 0; second <= 180; second += 15) {
        const double x = track.left() + (static_cast<double>(second) / 180.0) * track.width();
        painter.drawLine(QPointF(x, track.top()), QPointF(x, track.bottom()));
        painter.drawText(QPointF(x + 3, track.bottom() + 17), QStringLiteral("%1:%2")
            .arg(second / 60, 2, 10, QLatin1Char('0')).arg(second % 60, 2, 10, QLatin1Char('0')));
    }
    const double progress = timeline_.durationMs > 0.0 ? timeline_.playheadMs / timeline_.durationMs : 0.0;
    const double playheadX = track.left() + std::clamp(progress, 0.0, 1.0) * track.width();
    painter.setPen(QPen(QColor("#67b5f1"), 2));
    painter.drawLine(QPointF(playheadX, 22), QPointF(playheadX, track.bottom()));
    painter.setBrush(QColor("#67b5f1"));
    painter.drawPolygon({QPointF(playheadX - 6, 20), QPointF(playheadX + 6, 20), QPointF(playheadX, 28)});
    painter.setPen(QColor("#a9b7c8"));
    painter.drawText(QPointF(20, 18), QStringLiteral("TIMELINE  /  %1  %2")
        .arg(timeline_.playing ? QStringLiteral("PLAYING") : QStringLiteral("PAUSED"))
        .arg(QStringLiteral("%1 ms").arg(static_cast<int>(timeline_.playheadMs))));
}

void TimelineWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        const QRectF track = rect().adjusted(20, 34, -20, -24);
        const double ratio = std::clamp((event->position().x() - track.left()) / track.width(), 0.0, 1.0);
        emit seekRequested(ratio * timeline_.durationMs);
    }
    if (event->button() == Qt::MiddleButton) emit playPauseRequested();
    QWidget::mousePressEvent(event);
}

} // namespace pmp::ui
