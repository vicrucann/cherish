#ifndef MAINWINDOW
#define MAINWINDOW

#include <QMainWindow>
#include <QMdiArea>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QMenuBar>
#include <QUndoStack>
#include <QUndoView>
#include <QToolButton>
#include <QWidgetAction>

#include <osg/ref_ptr>

#include "rootscene.h"
#include "settings.h"
//#include "viewwidget.h"
#include "glwidget.h"
#include "bookmarkwidget.h"
#include "listwidget.h"

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

    void onMouseOrbit();
    void onMouseZoom();
    void onMousePan();
    void onMousePick();
    void onMouseErase();
    void onMouseDelete();
    void onMouseSketch();
    void onMouseOffset();
    void onMouseRotate();
    void onMouseMove();

    void loadImage();

private:
    GLWidget *createViewer(Qt::WindowFlags f = 0, int viewmode = 1);

    QDesktopWidget* _desktop;
    QMdiArea* _mdiArea;
    BookmarkWidget* _bookmarks;
    bool _tabletActive;

    QUndoStack * m_UndoStack; // http://doc.qt.io/qt-5/qtwidgets-tools-undoframework-example.html
    QUndoView * m_UndoView;

    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void createLayerManager();
    void createBookMark();

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *drawMenu;
    QMenu *cameraMenu;
    QMenu *toolsMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QMenu *toolPalettes;
    QMenu* _mSketchMenu;
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

    // ACTIONS ======================= //

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *print;

    QAction* m_ActionUndo;
    QAction* m_ActionRedo;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

    QAction *aboutAct;
    QAction *contactUs;

    QAction *axes;
    QAction *toolPalsandBox;

    QAction* _mActionSketch;
    QAction *line;
    QAction *arc;
    QAction *rectangle;

    QAction* _mActionOrbit;
    QAction* _mActionPan;
    QAction* _mActionZoom;
    QAction *zoomWindow;
    QAction *zoomExtents;
    QAction *previousCam;

    QAction* _mActionEraser;
    QAction* _mActionDelete;
    QAction* _mActionMove; //toolMove; // move current photo within current canvas
    QAction* _mActionPushStrokes;
    QAction* _mActionRotate;// toolRotate;
    QAction* _mActionRotateImage;
    QAction *toolScale;
    QAction* _mActionOffset; // offset current canvas along its normal

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

    QAction* _mActionStack;
    QAction* _mActionCircle;
    QAction* _mActionClone;
    QAction* _mActionXY;
    QAction* _mActionXZ;
    QAction* _mActionYZ;
    QWidgetAction* newCanvasAction;
    QMenu *newCanvasMenu;
    QToolButton *newCanvas;

    QAction* _mActionSelect;
    QAction* _mActionSelectPlane;
    QWidgetAction* selectAction;
    QMenu *selectMenu;
    QToolButton *select;

    QAction *loadImageAction;


    QCursor *m_currentCursor;

    QTableView *tableView;

    osg::ref_ptr<RootScene> _rootScene; // main scene graph

    QMenuBar* _menuBar;
    ListWidget* m_lw;

protected:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // MAINWINDOW

