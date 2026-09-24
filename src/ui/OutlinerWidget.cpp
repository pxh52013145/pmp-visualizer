#include "ui/OutlinerWidget.hpp"

#include <QHeaderView>
#include <QSignalBlocker>

namespace pmp::ui {

OutlinerWidget::OutlinerWidget(QWidget* parent)
    : QTreeWidget(parent) {
    setHeaderLabels({QStringLiteral("Scene"), QStringLiteral("State")});
    header()->setStretchLastSection(false);
    header()->setSectionResizeMode(0, QHeaderView::Stretch);
    header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    setRootIsDecorated(true);
    setIndentation(16);
    setSelectionMode(QAbstractItemView::SingleSelection);
    connect(this, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item, int column) {
        if (!item || item->data(0, Qt::UserRole).toString().isEmpty()) return;
        const QString id = item->data(0, Qt::UserRole).toString();
        if (column == 1) emit componentVisibilityToggled(id);
        else emit componentSelected(id);
    });
}

void OutlinerWidget::sync(const core::WorkbenchSnapshot& snapshot) {
    std::string signature = snapshot.selectedComponentId;
    for (const auto& component : snapshot.components) {
        signature.append("|").append(component.id).append(component.visible ? ":1" : ":0");
    }
    if (signature == lastSignature_) return;
    lastSignature_ = std::move(signature);
    const QSignalBlocker blocker(this);
    clear();
    auto* root = new QTreeWidgetItem(this, {QString::fromStdString(snapshot.sceneTitle), QStringLiteral("SCENE")});
    root->setData(0, Qt::UserRole, QString::fromStdString(snapshot.sceneId));
    root->setExpanded(true);
    for (const auto& component : snapshot.components) {
        auto* item = new QTreeWidgetItem(root, {QString::fromStdString(component.label),
                                                component.visible ? QStringLiteral("VISIBLE") : QStringLiteral("HIDDEN")});
        item->setData(0, Qt::UserRole, QString::fromStdString(component.id));
        item->setCheckState(1, component.visible ? Qt::Checked : Qt::Unchecked);
        item->setForeground(1, component.visible ? QColor("#79b6e8") : QColor("#687487"));
        if (component.selected) setCurrentItem(item);
    }
}

} // namespace pmp::ui
