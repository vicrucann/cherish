#ifndef MAINWINDOW
#define MAINWINDOW

#include <QMainWindow>
#include <QMdiArea>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QMenuBar>

#include <osg/ref_ptr>

#include "rootscene.h"
#include "settings.h"
#include "viewwidget.h"
#include "bookmarkwidget.h"

class QAction;
class QMenu;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();
    void SetDesktopWidget(QDesktopWidget* desktop, dureu::APPMODE mode = dureu::SCREEN_MIN);
public slots:
    void getTabletActivity(bool active);
signals:
    void sendTabletActivity(bool active);
    void sendStylusSketchStatus(bool sketch);
    void sendMouseMode(dureu::MOUSE_MODE mode);
private slots:
    void onCreateViewer();
    void onCreateDoubleViewer();
    void onCreateOutsideViewer();
    void onLoadCow();
    void onSetStylusSketchON();
    void onSetStylusSketchOFF();
    void onSetGloAxesON();
    void onSetGloAxesOFF();

    void onDeleteCanvas();
    void onDeleteCow();
    void onChangeSizeCanvas();

    void onMouseRotate();
    void onMouseZoom();
    void onMousePan();
    void onMousePick();
    void onMouseErase();
    void onMouseSketch();
private:
    ViewWidget *createViewer(Qt::WindowFlags f = 0, int viewmode = 1);

    QDesktopWidget* _desktop;
    QMdiArea* _mdiArea;
    BookmarkWidget* _bookmarks;
    bool _tabletActive;

    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void createLayerManager();

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *drawMenu;
    QMenu *cameraMenu;
    QMenu *toolsMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QMenu *toolPalettes;
    QMenu *lines;
    QMenu *arcs;
    QMenu *shapes;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *toolsToolBar;
    QToolBar *drawToolBar;
    QToolBar *cameraToolBar;
    QToolBar *styleToolBar;
    QToolBar *viewToolBar;
    QToolBar *windowToolBar;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *print;

    QAction *undo;
    QAction *redo;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

    QAction *aboutAct;
    QAction *contactUs;

    QAction *axes;
    QAction *toolPalsandBox;

    QAction *line;
    QAction *arc;
    QAction *rectangle;

    QAction *orbit;
    QAction *pan;
    QAction *zoom;
    QAction *zoomWindow;
    QAction *zoomExtents;
    QAction *previousCam;

    QAction *select;
    QAction *eraser;
    QAction *toolMove;
    QAction *toolRotate;
    QAction *toolScale;
    QAction *toolOffset;

    QAction *layers;

    QAction *allStyle;
    QAction *strokesStyle;
    QAction *imagesStyle;

    QAction *isoView;
    QAction *topView;
    QAction *frontView;
    QAction *rightView;
    QAction *backView;
    QAction *leftView;

    QTableWidget* m_pTableWidget;
    QStringList m_TableHeader;

    osg::ref_ptr<RootScene> _rootScene; // main scene graph

    QMenuBar* _menuBar;
};

#endif // MAINWINDOW

