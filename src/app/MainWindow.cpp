#include "app/MainWindow.hpp"

#include "core/VisualizerModel.hpp"
#include "render/Renderer.hpp"
#include "ui/InspectorWidget.hpp"
#include "ui/OutlinerWidget.hpp"
#include "ui/StudioTheme.hpp"
#include "ui/TimelineWidget.hpp"
#include "ui/ViewportWidget.hpp"

#include <QActionGroup>
#include <QDockWidget>
#include <QListWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

namespace pmp {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), model_(new core::WorkbenchModel()) {
    setStyleSheet(ui::studioStyleSheet());
    setWindowTitle(QStringLiteral("PMP Visualizer"));
    resize(1600, 980);
    setMinimumSize(1120, 720);
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks |
                   QMainWindow::AllowTabbedDocks);

    buildMenuBar();
    buildToolBar();
    buildDocks();
    viewport_ = new ui::ViewportWidget(model_, this);
    setCentralWidget(viewport_);
    connect(viewport_, &ui::ViewportWidget::frameTicked, this, [this] { syncUi(); });

    const RendererInfo renderer = Renderer::detect();
    statusBar()->showMessage(QStringLiteral("READY   |   %1   |   %2")
                                 .arg(renderer.backendName, renderer.status));
    syncUi();
}

void MainWindow::buildMenuBar() {
    auto* fileMenu = menuBar()->addMenu(QStringLiteral("File"));
    fileMenu->addAction(QStringLiteral("New Project"));
    fileMenu->addAction(QStringLiteral("Open Project"));
    fileMenu->addSeparator();
    fileMenu->addAction(QStringLiteral("Exit"), this, &QWidget::close);

    auto* editMenu = menuBar()->addMenu(QStringLiteral("Edit"));
    editMenu->addAction(QStringLiteral("Undo"));
    editMenu->addAction(QStringLiteral("Redo"));

    auto* viewMenu = menuBar()->addMenu(QStringLiteral("View"));
    viewMenu->addAction(QStringLiteral("Reset Layout"));
}

void MainWindow::buildToolBar() {
    auto* toolbar = addToolBar(QStringLiteral("Tools"));
    toolbar->setObjectName(QStringLiteral("studio-tools"));
    toolbar->setMovable(false);
    toolbar->setOrientation(Qt::Vertical);
    toolbar->setIconSize(QSize(18, 18));

    auto* group = new QActionGroup(toolbar);
    group->setExclusive(true);
    const QList<QPair<QString, core::ActiveTool>> tools = {
        {QStringLiteral("Select"), core::ActiveTool::Select},
        {QStringLiteral("Move"), core::ActiveTool::Move},
        {QStringLiteral("Scale"), core::ActiveTool::Scale},
        {QStringLiteral("Orbit"), core::ActiveTool::Orbit},
    };
    for (const auto& [label, tool] : tools) {
        auto* action = toolbar->addAction(label);
        action->setCheckable(true);
        action->setToolTip(label);
        group->addAction(action);
        if (tool == core::ActiveTool::Select) action->setChecked(true);
        connect(action, &QAction::triggered, this, [this, tool] { model_->setTool(tool); });
    }
    toolbar->addSeparator();
    auto* compose = toolbar->addAction(QStringLiteral("Compose"));
    compose->setCheckable(true);
    compose->setChecked(true);
    compose->setToolTip(QStringLiteral("Compose mode"));
    connect(compose, &QAction::triggered, this, [this] { model_->setMode(core::WorkbenchMode::Compose); });
    auto* preview = toolbar->addAction(QStringLiteral("Preview"));
    preview->setCheckable(true);
    preview->setToolTip(QStringLiteral("Preview mode"));
    connect(preview, &QAction::triggered, this, [this] { model_->setMode(core::WorkbenchMode::Preview); });
}

void MainWindow::buildDocks() {
    auto addDock = [this](const QString& title, const QString& objectName,
                          Qt::DockWidgetArea area, QWidget* content) {
        auto* dock = new QDockWidget(title, this);
        dock->setObjectName(objectName);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setWidget(content);
        addDockWidget(area, dock);
    };

    outliner_ = new ui::OutlinerWidget(this);
    inspector_ = new ui::InspectorWidget(this);
    timeline_ = new ui::TimelineWidget(this);
    addDock(QStringLiteral("OUTLINER"), QStringLiteral("outliner"), Qt::LeftDockWidgetArea, outliner_);
    addDock(QStringLiteral("INSPECTOR"), QStringLiteral("inspector"), Qt::RightDockWidgetArea, inspector_);
    addDock(QStringLiteral("TIMELINE"), QStringLiteral("timeline"), Qt::BottomDockWidgetArea, timeline_);

    auto* assets = new QListWidget(this);
    assets->addItems({QStringLiteral("SCENES"), QStringLiteral("SHADERS"), QStringLiteral("TEXTURES"), QStringLiteral("FONTS")});
    addDock(QStringLiteral("ASSETS"), QStringLiteral("assets"), Qt::LeftDockWidgetArea, assets);
    splitDockWidget(findChild<QDockWidget*>(QStringLiteral("outliner")),
                    findChild<QDockWidget*>(QStringLiteral("assets")), Qt::Vertical);

    connect(outliner_, &ui::OutlinerWidget::componentSelected, this, [this](const QString& id) {
        model_->selectComponent(id.toStdString());
        syncUi();
    });
    connect(outliner_, &ui::OutlinerWidget::componentVisibilityToggled, this, [this](const QString& id) {
        model_->toggleComponentVisibility(id.toStdString());
        syncUi();
    });
    connect(timeline_, &ui::TimelineWidget::seekRequested, this, [this](double position) {
        model_->seek(position);
        syncUi();
    });
    connect(timeline_, &ui::TimelineWidget::playPauseRequested, this, [this] {
        model_->setPlaying(!model_->snapshot().timeline.playing);
        syncUi();
    });
}

void MainWindow::syncUi() {
    if (!outliner_ || !inspector_ || !timeline_) return;
    const auto& snapshot = model_->snapshot();
    outliner_->sync(snapshot);
    inspector_->sync(snapshot);
    timeline_->sync(snapshot);
    statusBar()->showMessage(QStringLiteral("FRAME %1   |   %2 FPS   |   SCALE %3%   |   AUDIO %4")
        .arg(snapshot.frameNumber, 6, 10, QLatin1Char('0'))
        .arg(snapshot.actualFps, 0, 'f', 0)
        .arg(static_cast<int>(snapshot.renderScale * 100.0))
        .arg(static_cast<int>(snapshot.audio.smoothedEnergy * 100.0F)));
}

} // namespace pmp
