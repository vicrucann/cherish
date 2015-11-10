#ifndef MAINWINDOW
#define MAINWINDOW

#include <QMainWindow>
#include <QMdiArea>

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Drawable>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();
public slots:
    void getTabletActivity(bool active);
private slots:
    void onCreateViewer();
signals:
    void sendTabletActivity(bool active);
private:
    QMdiArea* _mdiArea;
    bool _tabletActive;

    osg::ref_ptr<osg::Group> _root; // main graph that also includes HUD camera and such
    osg::ref_ptr<osg::Group> _scene;  // scene graph only, includes canvases and their drawables only
};

#endif // MAINWINDOW

