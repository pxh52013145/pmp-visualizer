#pragma once

#include <QMainWindow>
#include <QString>
#include <QStringList>

namespace pmp::core {
class WorkbenchModel;
}

namespace pmp::ui {
class InspectorWidget;
class OutlinerWidget;
class TimelineWidget;
class ViewportWidget;
}

namespace pmp {

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void buildMenuBar();
    void buildToolBar();
    void buildDocks();
    void syncUi();

    core::WorkbenchModel* model_ = nullptr;
    ui::ViewportWidget* viewport_ = nullptr;
    ui::OutlinerWidget* outliner_ = nullptr;
    ui::InspectorWidget* inspector_ = nullptr;
    ui::TimelineWidget* timeline_ = nullptr;
};

} // namespace pmp
