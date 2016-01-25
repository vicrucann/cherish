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

#include "RootScene.h"
#include "Settings.h"
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

    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileImage();
    void onFileClose();
    void onFileExit();

    void onCut();
    void onCopy();
    void onPaste();

    void onCameraOrbit();
    void onCameraZoom();
    void onCameraPan();
    void onSelect();
    void onErase();
    void onDelete();
    void onSketch();

    void onNewCanvasClone();
    void onNewCanvasXY();
    void onNewCanvasYZ();
    void onNewCanvasXZ();
    void onNewCanvasStandard();
    void onNewCanvasCoaxial();
    void onNewCanvasParallel();
    void onNewCanvasRing();

    void onCanvasOffset();
    void onCanvasRotate();

    void onImageMove();
    void onImageRotate();
    void onImageScale();
    void onImageFlip();
    void onImagePush();

    void onStrokePush();

private:
    GLWidget*   createViewer(Qt::WindowFlags f = 0, int viewmode = 1);
    void        initializeActions();
    void        initializeMenus();
    void        initializeToolbars();

    QDesktopWidget* m_desktop;
    QMdiArea*       m_mdiArea;
    GLWidget*       m_glWidget;

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

