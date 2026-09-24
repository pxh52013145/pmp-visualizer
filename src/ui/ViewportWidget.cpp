#include "ui/ViewportWidget.hpp"

#include <QDateTime>
#include <QPainter>
#include <QResizeEvent>
#include <QTimer>

#include <algorithm>
#include <cmath>

namespace pmp::ui {

ViewportWidget::ViewportWidget(core::WorkbenchModel* model, QWidget* parent)
    : QWidget(parent), model_(model), timer_(new QTimer(this)) {
    setObjectName(QStringLiteral("viewport"));
    setAttribute(Qt::WA_OpaquePaintEvent);
    setFocusPolicy(Qt::StrongFocus);
    timer_->setTimerType(Qt::PreciseTimer);
    timer_->setInterval(16);
    connect(timer_, &QTimer::timeout, this, [this] {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        const double delta = lastTickMs_ > 0 ? static_cast<double>(now - lastTickMs_) : 16.0;
        lastTickMs_ = now;
        model_->advance(std::clamp(delta, 1.0, 100.0));
        update();
        emit frameTicked();
    });
    timer_->start();
}

void ViewportWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    update();
}

void ViewportWidget::paintEvent(QPaintEvent*) {
    const auto& snapshot = model_->snapshot();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor("#0b0e13"));

    const QRectF viewport = rect().adjusted(22, 38, -22, -22);
    painter.setPen(QPen(QColor("#1b2330"), 1));
    for (int x = static_cast<int>(viewport.left()); x < viewport.right(); x += 40) {
        painter.drawLine(x, viewport.top(), x, viewport.bottom());
    }
    for (int y = static_cast<int>(viewport.top()); y < viewport.bottom(); y += 40) {
        painter.drawLine(viewport.left(), y, viewport.right(), y);
    }

    painter.setPen(QPen(QColor("#536174"), 1));
    painter.drawText(QPointF(24, 22), QStringLiteral("SCENE / %1").arg(QString::fromStdString(snapshot.sceneTitle)).toUpper());
    painter.setPen(QColor("#667489"));
    painter.drawText(QPointF(width() - 190, 22), QStringLiteral("%1 FPS   %2 ms")
        .arg(snapshot.actualFps, 0, 'f', 0).arg(snapshot.frameTimeMs, 0, 'f', 1));

    const QPointF center = viewport.center();
    const float pulse = 1.0F + snapshot.audio.smoothedEnergy * 0.15F;
    const double radius = std::min(viewport.width(), viewport.height()) * 0.28 * pulse;
    painter.setPen(QPen(QColor(73, 137, 202, 70), 1));
    painter.drawEllipse(center, radius * 1.26, radius * 1.26);
    painter.setPen(QPen(QColor(79, 156, 224, 170), 1.2));
    painter.drawEllipse(center, radius, radius);

    const int bars = static_cast<int>(snapshot.audio.spectrum.size());
    const double barWidth = radius * 2.0 / bars;
    painter.setPen(Qt::NoPen);
    for (int index = 0; index < bars; ++index) {
        const double value = snapshot.audio.spectrum[static_cast<size_t>(index)];
        const double barHeight = 6.0 + value * radius * 0.62;
        const QColor color = QColor::fromHsv(205 - static_cast<int>(value * 38.0), 170, 230, 180);
        painter.setBrush(color);
        const QRectF bar(center.x() - radius + index * barWidth, center.y() - radius - barHeight,
                         std::max(1.0, barWidth - 1.0), barHeight);
        painter.drawRoundedRect(bar, 1.5, 1.5);
        painter.drawRoundedRect(QRectF(bar.left(), center.y() + radius, bar.width(), bar.height()), 1.5, 1.5);
    }

    painter.setBrush(QColor("#151d29"));
    painter.setPen(QPen(QColor("#5d9bd0"), 1));
    painter.drawEllipse(center, 74.0, 74.0);
    painter.setPen(QColor("#dce8f4"));
    painter.drawText(QRectF(center.x() - 70, center.y() - 23, 140, 24), Qt::AlignCenter,
                     QStringLiteral("%1%").arg(static_cast<int>(snapshot.audio.smoothedEnergy * 100.0F)));
    painter.setPen(QColor("#718096"));
    painter.drawText(QRectF(center.x() - 70, center.y() + 8, 140, 20), Qt::AlignCenter,
                     QStringLiteral("AUDIO INPUT"));

    for (const auto& component : snapshot.components) {
        if (!component.visible || !component.selected) continue;
        painter.setPen(QPen(QColor("#78c2ff"), 1, Qt::DashLine));
        painter.drawEllipse(center, radius * component.response + 8, radius * component.response + 8);
        painter.setPen(QColor("#9ecfff"));
        painter.drawText(QPointF(center.x() + radius + 16, center.y() - radius),
                         QStringLiteral("SELECTED  /  %1").arg(QString::fromStdString(component.label).toUpper()));
    }

    painter.setPen(QColor("#536174"));
    painter.drawText(QPointF(24, height() - 8), QStringLiteral("%1  |  %2  |  SCALE %3%")
        .arg(snapshot.mode == core::WorkbenchMode::Compose ? QStringLiteral("COMPOSE") : QStringLiteral("PREVIEW"))
        .arg(snapshot.tool == core::ActiveTool::Select ? QStringLiteral("SELECT") : QStringLiteral("TRANSFORM"))
        .arg(static_cast<int>(snapshot.renderScale * 100.0)));
}

} // namespace pmp::ui
