#pragma once

#include "core/VisualizerModel.hpp"

#include <QWidget>

class QTimer;

namespace pmp::ui {

class ViewportWidget final : public QWidget {
    Q_OBJECT

public:
    explicit ViewportWidget(core::WorkbenchModel* model, QWidget* parent = nullptr);
    QSize minimumSizeHint() const override { return QSize(640, 420); }

signals:
    void frameTicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    core::WorkbenchModel* model_;
    QTimer* timer_;
    qint64 lastTickMs_ = 0;
};

} // namespace pmp::ui

