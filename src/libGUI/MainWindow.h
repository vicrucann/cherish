#ifndef MAINWINDOW
#define MAINWINDOW

#include <QMainWindow>
#include <QMdiArea>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QTabWidget>
#include <QListWidget>
#include <QMenuBar>
#include <QUndoStack>
#include <QUndoView>
#include <QToolButton>
#include <QWidgetAction>

#include <osg/ref_ptr>

#include "RootScene.h"
#include "Settings.h"
#include "GLWidget.h"
#include "BookmarkWidget.h"
#include "CanvasWidget.h"
#include "Data.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();
    void SetDesktopWidget(QDesktopWidget* desktop, dureu::APPMODE mode = dureu::SCREEN_MIN);

public slots:
    void getTabletActivity(bool active);
    void recievedRequestUpdate();
    void recieveAutoSwitchMode(dureu::MOUSE_MODE mode);
    void recieveBookmark(int row);
    void onDeleteBookmark(const QModelIndex &index);
    void onMoveBookmark(const QModelIndex &index);

signals:
    void sendTabletActivity(bool active);
    void sendMouseMode(dureu::MOUSE_MODE mode);

private slots:
    //void onCreateViewer();

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
    void onTools();

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
    void onImageFlipH();
    void onImageFlipV();
    void onImagePush();

    void onStrokesPush();

    void onBookmark();

private:
    //GLWidget*   createViewer(Qt::WindowFlags f = 0, int viewmode = 1);
    void        initializeActions();
    void        initializeMenus();
    void        initializeToolbars();
    void        initializeCallbacks(); // for ctor
    void        reinitializeCallbacks(); // for open file

    QDesktopWidget* m_desktop;
    QMdiArea*       m_mdiArea;

    QTabWidget*     m_tabWidget;
    BookmarkWidget*    m_bookmarkWidget;
    CanvasWidget*    m_canvasWidget;


    // http://doc.qt.io/qt-5/qtwidgets-tools-undoframework-example.html
    QUndoStack*     m_undoStack;
    QUndoView*      m_undoView;

    QMenuBar*       m_menuBar;
    osg::ref_ptr<RootScene> m_rootScene; // main scene graph
    QUndoStack* m_viewStack;
    GLWidget*       m_glWidget;

    // FILE actions
    QAction * m_actionNewFile, * m_actionClose, * m_actionExit,
            * m_actionImportImage, * m_actionOpenFile, * m_actionSaveFile;

    // EDIT actions
    QAction * m_actionUndo, * m_actionRedo, * m_actionCut, * m_actionCopy,
            * m_actionPaste, * m_actionDelete,
            * m_actionTools;

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
            , * m_actionImageMove, * m_actionImageRotate, * m_actionImageScale, * m_actionImageFlipV
            , * m_actionImageFlipH, * m_actionImagePush
            // Edit Strokes
            , * m_actionStrokesPush
    ;

    /* VIEWER actions */
    QAction * m_actionPrevView, * m_actionNextView, * m_actionBookmark;

    // QToolButton *select;
    // QWidgetAction* selectAction;
};

#endif // MAINWINDOW

