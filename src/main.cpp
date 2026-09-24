#include "app/MainWindow.hpp"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    QApplication::setApplicationName(QStringLiteral("PMP Visualizer"));
    QApplication::setApplicationVersion(QStringLiteral("0.1.0"));

    pmp::MainWindow window;
    window.show();
    return application.exec();
}

