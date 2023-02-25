// Speed Typer
// Dominic Gasperini

// includes
#include "MainWindow.h"
#include <QApplication>

// main!
int main(int argc, char *argv[])
{
    // Init UI
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // Exit on close
    return a.exec();
}
