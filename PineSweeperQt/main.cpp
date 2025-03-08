#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // For QSettings
    QCoreApplication::setOrganizationName("CVN");
    QCoreApplication::setOrganizationDomain("github.com");
    QCoreApplication::setApplicationName("PineSweeperQt");

    MainWindow w;
    w.setWindowTitle("PineSweeperQt");
    w.show();
    w.resize(1600, 900);
    return a.exec();
}
