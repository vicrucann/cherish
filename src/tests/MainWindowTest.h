#ifndef MAINWINDOWTEST_H
#define MAINWINDOWTEST_H

#include <QObject>
#include <QTest>

#include "BaseGuiTest.h"

class MainWindowTest : public BaseGuiTest
{
    Q_OBJECT

private slots:
    void testToolsOnOff();
};

#endif // MAINWINDOWTEST_H
