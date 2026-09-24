#pragma once

#include <QString>

namespace pmp::ui {

inline QString studioStyleSheet() {
    return QStringLiteral(R"(
        QWidget { color: #d7dce5; background: #15181e; font-family: "Segoe UI"; font-size: 12px; }
        QMainWindow { background: #101217; }
        QMenuBar { background: #111419; border-bottom: 1px solid #2a303b; padding: 2px 6px; }
        QMenuBar::item { padding: 5px 9px; color: #b9c0cb; }
        QMenuBar::item:selected { background: #273143; color: #ffffff; }
        QMenu { background: #1a1f27; border: 1px solid #323b49; }
        QMenu::item { padding: 7px 28px 7px 12px; }
        QMenu::item:selected { background: #2d3b53; }
        QToolBar { background: #151920; border: 0; border-right: 1px solid #2b323e; spacing: 4px; padding: 6px 5px; }
        QToolButton { color: #aab3c1; border: 1px solid transparent; border-radius: 3px; padding: 7px; }
        QToolButton:hover { color: #ffffff; background: #252e3d; border-color: #3b4d69; }
        QToolButton:checked { color: #77b9ff; background: #21344e; border-color: #3f78b1; }
        QDockWidget { titlebar-close-icon: none; titlebar-normal-icon: none; font-weight: 600; }
        QDockWidget::title { background: #1a1f27; border: 1px solid #2c3440; padding: 7px 9px; text-align: left; }
        QListWidget, QTreeWidget, QTableWidget { background: #171b21; border: 1px solid #2b333f; outline: 0; }
        QListWidget::item, QTreeWidget::item { padding: 5px 4px; border-bottom: 1px solid #202630; }
        QListWidget::item:selected, QTreeWidget::item:selected { background: #263a56; color: #ffffff; }
        QHeaderView::section { background: #1d232c; color: #8995a7; border: 0; padding: 5px; }
        QLineEdit, QDoubleSpinBox, QSpinBox, QComboBox { background: #11151b; border: 1px solid #303a48; border-radius: 2px; padding: 5px; }
        QLineEdit:focus, QDoubleSpinBox:focus, QSpinBox:focus, QComboBox:focus { border-color: #4f91d5; }
        QSlider::groove:horizontal { height: 3px; background: #374251; }
        QSlider::handle:horizontal { width: 11px; margin: -4px 0; border-radius: 5px; background: #68a9e8; }
        QStatusBar { background: #111419; border-top: 1px solid #2a303b; color: #7f8b9a; }
        QScrollBar:vertical { background: #151920; width: 10px; }
        QScrollBar::handle:vertical { background: #374252; min-height: 24px; }
    )");
}

} // namespace pmp::ui

