#pragma once

#include <QMainWindow>
#include <QString>
#include <QStringList>

namespace pmp {

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void buildMenuBar();
    void buildDocks();
    QWidget* createViewport();
    QWidget* createListPanel(const QString& title, const QStringList& rows);
};

} // namespace pmp
