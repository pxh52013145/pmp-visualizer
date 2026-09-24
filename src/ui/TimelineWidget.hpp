#pragma once

#include "core/VisualizerModel.hpp"

#include <QWidget>

namespace pmp::ui {

class TimelineWidget final : public QWidget {
    Q_OBJECT

public:
    explicit TimelineWidget(QWidget* parent = nullptr);
    QSize minimumSizeHint() const override { return QSize(640, 120); }
    void sync(const core::WorkbenchSnapshot& snapshot);

signals:
    void seekRequested(double playheadMs);
    void playPauseRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    core::TimelineState timeline_;
};

} // namespace pmp::ui

