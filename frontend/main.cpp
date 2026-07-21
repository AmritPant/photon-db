#include <QApplication>
#include "mainwindow.h"

// Every Qt Widgets app starts the same way:
// 1) create a QApplication (manages the event loop -- like your poll() loop,
//    but Qt hides it from you)
// 2) create your window
// 3) call app.exec(), which blocks and processes events (clicks, key presses,
//    socket data arriving, etc.) until the window is closed
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}