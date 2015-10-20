#include "examplewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication qapp(argc, argv);

    ExampleWindow egl;
    egl.resize(640,480);
    egl.show();
    egl.setAnimating(true);

    return qapp.exec();
}
