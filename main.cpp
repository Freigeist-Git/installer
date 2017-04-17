#include "mainwindow.h"

int main(int argc, char *argv[]) {
    
    QApplication app(argc, argv);

    Mainwindow window;
    

    window.resize(350, 350);
    window.setMaximumSize(500, 450);
    window.setMinimumSize(300, 250);
    window.show();

    return app.exec();
}