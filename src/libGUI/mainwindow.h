#ifndef MAINWINDOW
#define MAINWINDOW

#include <QMainWindow>
#include <QMdiArea>
#include <QDesktopWidget>

#include <osg/ref_ptr>

#include "rootscene.h"
#include "settings.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();
    void SetDesktopWidget(QDesktopWidget* desktop, dureu::APPMODE mode = dureu::APPMODE::MAX_SCREEN);
public slots:
    void getTabletActivity(bool active);
signals:
    void sendTabletActivity(bool active);
    void sendStylusSketchStatus(bool sketch);
private slots:
    void onCreateViewer();
    void onCreateDoubleViewer();
    void onCreateOutsideViewer();
    void onLoadCow();
    void onSetStylusSketchON();
    void onSetStylusSketchOFF();
    void onSetGloAxesON();
    void onSetGloAxesOFF();
private:
    QDesktopWidget* _desktop;
    QMdiArea* _mdiArea;
    bool _tabletActive;

    osg::ref_ptr<RootScene> _rootScene; // main scene graph
};

#endif // MAINWINDOW

