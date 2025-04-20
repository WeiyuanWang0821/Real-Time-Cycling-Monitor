////////////////////////////////////////
// File: main.cpp
// Description: Main entry point for Qt GUI application
//              Initializes QApplication and displays main window
////////////////////////////////////////
#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);          // Create the Qt application instance
    Widget w;                             // Instantiate the main window
    w.show();                             // Show the main window
    return a.exec();                      // Enter Qt event loop
}
