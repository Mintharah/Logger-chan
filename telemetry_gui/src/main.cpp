#include <QApplication>
#include <QFont>
#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("TelemetryLogger Server");
    app.setOrganizationName("Yasmine");
    QFont f("Segoe UI", 10);
    f.setStyleHint(QFont::SansSerif);
    app.setFont(f);
    MainWindow w;
    w.show();
    return app.exec();
}
