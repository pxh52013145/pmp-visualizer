#include "ui/InspectorWidget.hpp"

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

#include <algorithm>

namespace pmp::ui {

namespace {
QProgressBar* makeMeter(QWidget* parent) {
    auto* meter = new QProgressBar(parent);
    meter->setRange(0, 1000);
    meter->setTextVisible(false);
    meter->setFixedHeight(6);
    return meter;
}
}

InspectorWidget::InspectorWidget(QWidget* parent)
    : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(12);

    auto* objectBox = new QGroupBox(QStringLiteral("OBJECT"), this);
    auto* objectForm = new QFormLayout(objectBox);
    selectionLabel_ = new QLabel(QStringLiteral("Nothing selected"), objectBox);
    positionLabel_ = new QLabel(QStringLiteral("0.00, 0.00, 0.00"), objectBox);
    responseLabel_ = new QLabel(QStringLiteral("1.00x"), objectBox);
    objectForm->addRow(QStringLiteral("Selection"), selectionLabel_);
    objectForm->addRow(QStringLiteral("Position"), positionLabel_);
    objectForm->addRow(QStringLiteral("Response"), responseLabel_);
    layout->addWidget(objectBox);

    auto* analysisBox = new QGroupBox(QStringLiteral("AUDIO ANALYSIS"), this);
    auto* analysisForm = new QFormLayout(analysisBox);
    energyBar_ = makeMeter(analysisBox);
    bassBar_ = makeMeter(analysisBox);
    midBar_ = makeMeter(analysisBox);
    trebleBar_ = makeMeter(analysisBox);
    analysisForm->addRow(QStringLiteral("Energy"), energyBar_);
    analysisForm->addRow(QStringLiteral("Bass"), bassBar_);
    analysisForm->addRow(QStringLiteral("Mid"), midBar_);
    analysisForm->addRow(QStringLiteral("Treble"), trebleBar_);
    layout->addWidget(analysisBox);
    layout->addStretch();
}

void InspectorWidget::sync(const core::WorkbenchSnapshot& snapshot) {
    const auto selected = std::find_if(snapshot.components.begin(), snapshot.components.end(),
                                       [](const auto& component) { return component.selected; });
    if (selected == snapshot.components.end()) {
        selectionLabel_->setText(QStringLiteral("Nothing selected"));
        positionLabel_->setText(QStringLiteral("-"));
        responseLabel_->setText(QStringLiteral("-"));
    } else {
        selectionLabel_->setText(QString::fromStdString(selected->label));
        positionLabel_->setText(QStringLiteral("%1, %2, %3")
            .arg(selected->transform.x, 0, 'f', 2).arg(selected->transform.y, 0, 'f', 2).arg(selected->transform.z, 0, 'f', 2));
        responseLabel_->setText(QStringLiteral("%1x").arg(selected->response, 0, 'f', 2));
    }
    energyBar_->setValue(static_cast<int>(snapshot.audio.smoothedEnergy * 1000.0F));
    bassBar_->setValue(static_cast<int>(snapshot.audio.bass * 1000.0F));
    midBar_->setValue(static_cast<int>(snapshot.audio.mid * 1000.0F));
    trebleBar_->setValue(static_cast<int>(snapshot.audio.treble * 1000.0F));
}

} // namespace pmp::ui
