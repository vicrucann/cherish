#include <iostream>
#include "dureuapplication.h"
#include "mainwindow.h"
#include <QObject>

int main(int argc, char** argv)
{
    std::cout << "Dura-Europus In Situ: 3D reconstruction of historical sites." << std::endl;

    DureuApplication dura_app(argc, argv);
    MainWindow mwin;
    QObject::connect(&dura_app, SIGNAL(sendTabletActivity(bool)),
                     &mwin, SLOT(getTabletActivity(bool)));
    mwin.show();

    return dura_app.exec();
}

