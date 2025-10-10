#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("SimpleQtApp");
    QApplication::setOrganizationName("MyOrg");

    MainWindow w;
    w.resize(800, 500);
    w.show();
    return app.exec();
}