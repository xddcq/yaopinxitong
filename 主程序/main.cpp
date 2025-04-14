// main.cpp
//star 2025.4.2 22:00
#include "mainwindow.h"
#include "warehouse.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Warehouse warehouse;
    if (!warehouse.loadFromFile()) {
        qDebug() << "Failed to load data from file or no data exists";
    }

    MainWindow w;
    w.show();

    int result = a.exec();

    if (!warehouse.saveToFile()) {
        qDebug() << "Failed to save data to file";
    }

    return result;
}
