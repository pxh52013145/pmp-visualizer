#include "app/MainWindow.hpp"

#include "render/Renderer.hpp"

#include <QDockWidget>
#include <QLabel>
#include <QListWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

namespace pmp {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("PMP Visualizer"));
    resize(1440, 900);
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks |
                   QMainWindow::AllowTabbedDocks);
    buildMenuBar();
    buildDocks();
    setCentralWidget(createViewport());

    const RendererInfo renderer = Renderer::detect();
    statusBar()->showMessage(QStringLiteral("Renderer: %1 | %2")
                                 .arg(renderer.backendName, renderer.status));
}

void MainWindow::buildMenuBar() {
    auto* fileMenu = menuBar()->addMenu(QStringLiteral("File"));
    fileMenu->addAction(QStringLiteral("New Project"));
    fileMenu->addAction(QStringLiteral("Open Project"));
    fileMenu->addSeparator();
    fileMenu->addAction(QStringLiteral("Exit"), this, &QWidget::close);

    auto* viewMenu = menuBar()->addMenu(QStringLiteral("View"));
    viewMenu->addAction(QStringLiteral("Reset Layout"));
}

void MainWindow::buildDocks() {
    auto addDock = [this](const QString& title, Qt::DockWidgetArea area, QWidget* content) {
        auto* dock = new QDockWidget(title, this);
        dock->setObjectName(title.toLower().replace(' ', '-'));
        dock->setWidget(content);
        addDockWidget(area, dock);
    };

    addDock(QStringLiteral("Outliner"), Qt::LeftDockWidgetArea,
            createListPanel(QStringLiteral("Scene"), {QStringLiteral("Scene"), QStringLiteral("Audio Input"),
                                                       QStringLiteral("Shader"), QStringLiteral("Output")}));
    addDock(QStringLiteral("Inspector"), Qt::RightDockWidgetArea,
            createListPanel(QStringLiteral("Properties"), {QStringLiteral("Transform"), QStringLiteral("Parameters"),
                                                            QStringLiteral("Performance")}));
    addDock(QStringLiteral("Assets"), Qt::LeftDockWidgetArea,
            createListPanel(QStringLiteral("Project"), {QStringLiteral("Scenes"), QStringLiteral("Shaders"),
                                                         QStringLiteral("Textures")}));
    addDock(QStringLiteral("Timeline"), Qt::BottomDockWidgetArea,
            createListPanel(QStringLiteral("Transport"), {QStringLiteral("00:00:00.000"), QStringLiteral("Loop: off")}));
}

QWidget* MainWindow::createViewport() {
    auto* viewport = new QWidget(this);
    viewport->setObjectName(QStringLiteral("render-viewport"));
    viewport->setStyleSheet(QStringLiteral("#render-viewport { background: #111318; }"));
    auto* layout = new QVBoxLayout(viewport);
    layout->setContentsMargins(24, 24, 24, 24);

    auto* title = new QLabel(QStringLiteral("Viewport"), viewport);
    title->setStyleSheet(QStringLiteral("color: #9da7b5; font-size: 16px;"));
    layout->addWidget(title, 0, Qt::AlignLeft | Qt::AlignTop);
    layout->addStretch();
    auto* message = new QLabel(QStringLiteral("Renderer device initialization is the next milestone."), viewport);
    message->setStyleSheet(QStringLiteral("color: #5e6878; font-size: 14px;"));
    message->setAlignment(Qt::AlignCenter);
    layout->addWidget(message);
    layout->addStretch();
    return viewport;
}

QWidget* MainWindow::createListPanel(const QString& title, const QStringList& rows) {
    auto* panel = new QWidget(this);
    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(8, 8, 8, 8);
    auto* heading = new QLabel(title, panel);
    heading->setStyleSheet(QStringLiteral("font-weight: 600;"));
    layout->addWidget(heading);
    auto* list = new QListWidget(panel);
    list->addItems(rows);
    layout->addWidget(list);
    return panel;
}

} // namespace pmp

