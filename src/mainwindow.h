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
    osg::Drawable* createAxes(const osg::Vec3& corner,const osg::Vec3& xdir,const osg::Vec3& ydir,const osg::Vec3& zdir);

    QMdiArea* _mdiArea;
    bool _tabletActive;

    osg::ref_ptr<osg::Group> _root; // scene graph
    osg::ref_ptr<osg::Geode> _axes;
};

#endif // MAINWINDOW

