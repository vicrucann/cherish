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
#include "glwidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();
    void SetDesktopWidget(QDesktopWidget* desktop, dureu::APPMODE mode = dureu::SCREEN_MIN);

public slots:
    void getTabletActivity(bool active);

signals:
    void sendTabletActivity(bool active);
    void sendMouseMode(dureu::MOUSE_MODE mode);

private slots:
    void onCreateViewer();

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

    void onFileImage();

private:
    GLWidget*   createViewer(Qt::WindowFlags f = 0, int viewmode = 1);
    void        initializeActions();
    void        initializeMenus();

    QDesktopWidget* m_desktop;
    QMdiArea*       m_mdiArea;

    // http://doc.qt.io/qt-5/qtwidgets-tools-undoframework-example.html
    QUndoStack*     m_undoStack;
    QUndoView*      m_undoView;

    QMenuBar*       m_menuBar;
    osg::ref_ptr<RootScene> m_rootScene; // main scene graph

    // FILE actions
    QAction * m_actionNewFile, * m_actionClose, * m_actionExit,
            * m_actionImportImage, * m_actionOpenFile, * m_actionSaveFile;

    // EDIT actions
    QAction * m_actionUndo, * m_actionRedo, * m_actionCut, * m_actionCopy,
            * m_actionPaste, * m_actionDelete;

    // SCENE actions
    QAction * m_actionSketch, * m_actionEraser, * m_actionSelect
            // Camera navigation
            , * m_actionOrbit, * m_actionPan, * m_actionZoom
            // New Canvas sub-menu
            , * m_actionCanvasClone, * m_actionCanvasXY, * m_actionCanvasYZ, * m_actionCanvasXZ
            // New Canvas Set sub-menu
            , * m_actionSetStandard, * m_actionSetParallel, * m_actionSetCoaxial, * m_actionSetRing
            // Edit Canvas
            , * m_actionCanvasOffset, * m_actionCanvasRotate
            // Edit Image
            , * m_actionImageMove, * m_actionImageRotate, * m_actionImageScale, * m_actionImageFlip, * m_actionImagePush
            // Edit Strokes
            , * m_actionStrokesPush
    ;


    // QToolButton *select;
    // QWidgetAction* selectAction;
};

#endif // MAINWINDOW

