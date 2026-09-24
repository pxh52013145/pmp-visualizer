#pragma once

#include "core/VisualizerModel.hpp"

#include <QTreeWidget>

#include <string>

namespace pmp::ui {

class OutlinerWidget final : public QTreeWidget {
    Q_OBJECT

public:
    explicit OutlinerWidget(QWidget* parent = nullptr);
    void sync(const core::WorkbenchSnapshot& snapshot);

signals:
    void componentSelected(const QString& id);
    void componentVisibilityToggled(const QString& id);

private:
    std::string lastSignature_;
};

} // namespace pmp::ui
