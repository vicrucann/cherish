#ifndef BASETEST_H
#define BASETEST_H

#include <QObject>
#include <QTest>
#include <QtGlobal>
#include <QScopedPointer>

#include "MainWindow.h"
#include "Settings.h"

class MainWindow;

class BaseTest : public QObject
{
    Q_OBJECT
public:
    explicit BaseTest(QObject *parent = 0);

private slots:
    void init();
    void cleanup();
    void trial();

protected:
//    QScopedPointer<MainWindow> m_window;
};

#endif // BASETEST_H
