#pragma once

#include "core/VisualizerModel.hpp"

#include <QWidget>

class QLabel;
class QProgressBar;

namespace pmp::ui {

class InspectorWidget final : public QWidget {
    Q_OBJECT

public:
    explicit InspectorWidget(QWidget* parent = nullptr);
    void sync(const core::WorkbenchSnapshot& snapshot);

private:
    QLabel* selectionLabel_ = nullptr;
    QLabel* positionLabel_ = nullptr;
    QLabel* responseLabel_ = nullptr;
    QProgressBar* energyBar_ = nullptr;
    QProgressBar* bassBar_ = nullptr;
    QProgressBar* midBar_ = nullptr;
    QProgressBar* trebleBar_ = nullptr;
};

} // namespace pmp::ui

