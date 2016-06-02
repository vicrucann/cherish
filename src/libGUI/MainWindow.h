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
#include "CameraProperties.h"
#include "ListWidget.h"
#include "TreeWidget.h"
#include "Data.h"
#include "../tests/BaseTest.h"

/*! \class MainWindow
 * Class description
*/
class MainWindow : public QMainWindow {
    Q_OBJECT

    friend class BaseTest; // to do unit tests using private methods

public:
    explicit MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    void SetDesktopWidget(QDesktopWidget* desktop, cher::APPMODE mode = cher::SCREEN_MIN);
    RootScene* getRootScene() const;

public slots:
    void getTabletActivity(bool active);
    void recievedRequestUpdate();
    void recieveAutoSwitchMode(cher::MOUSE_MODE mode);
    void onRequestBookmarkSet(int row);
    void onDeleteBookmark(const QModelIndex &index);
    void onDeleteCanvas(const QModelIndex& index);
    void onDeletePhoto(const QModelIndex& index);
    void onVisibilitySetCanvas(const QModelIndex& index);
    void onMoveBookmark(const QModelIndex &index);
    void onBookmarkAddedToWidget(const QModelIndex &, int first, int last);
    void onBookmarkRemovedFromWidget(const QModelIndex &, int first, int last);
    void slotMouseModeSet(cher::MOUSE_MODE mode);
    void slotPhotoTransparencyPlus(const QModelIndex& index);
    void slotPhotoTransparencyMinus(const QModelIndex& index);
    void slotPhotoPushed(int parent, int start, int, int destination, int);
    void onRequestSceneData(entity::SceneState* state);

signals:
    void signalTabletActivity(bool active);
    void signalMouseMode(cher::MOUSE_MODE mode);

private slots:
    //void onCreateViewer();

    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileExport();
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
    void onCameraAperture();

    void onSelect();
    void onErase();
    void onDelete();
    void onSketch();

    void onNewCanvasClone();
    void onNewCanvasXY();
    void onNewCanvasYZ();
    void onNewCanvasXZ();
    void onNewCanvasOrtho();
    void onNewCanvasSeparate();
    void onNewCanvasStandard();
    void onNewCanvasCoaxial();
    void onNewCanvasParallel();
    void onNewCanvasRing();

//    void onCanvasOffset();
//    void onCanvasRotate();
    void onCanvasEdit();

    void onImageMove();
    void onImageRotate();
    void onImageScale();
    void onImageFlipH();
    void onImageFlipV();

    void onStrokesPush();

    void onBookmark();

private:
    //GLWidget*   createViewer(Qt::WindowFlags f = 0, int viewmode = 1);
    void        initializeActions();
    void        initializeMenus();
    void        initializeToolbars();
    void        initializeCallbacks(); // for ctor

    QDesktopWidget* m_desktop;
    QMdiArea*       m_mdiArea;

    QTabWidget*     m_tabWidget;
    BookmarkWidget*    m_bookmarkWidget;
    CanvasPhotoWidget*    m_canvasWidget;


    // http://doc.qt.io/qt-5/qtwidgets-tools-undoframework-example.html
    QUndoStack*     m_undoStack;

    QMenuBar*       m_menuBar;
    osg::ref_ptr<RootScene> m_rootScene; // main scene graph
    QUndoStack* m_viewStack;
    GLWidget*       m_glWidget;

    // FILE actions
    QAction * m_actionNewFile, * m_actionClose, * m_actionExit,
            * m_actionImportImage, * m_actionOpenFile, * m_actionSaveFile,
            * m_actionSaveAsFile, * m_actionExportAs;

    // EDIT actions
    QAction * m_actionUndo, * m_actionRedo, * m_actionCut, * m_actionCopy,
            * m_actionPaste, * m_actionDelete,
            * m_actionTools;

    /* CAMERA actions */
    QAction * m_actionOrbit, * m_actionPan, * m_actionZoom
            , * m_actionPrevView, * m_actionNextView, * m_actionBookmark
            , * m_actionCameraSettings;

    // SCENE actions
    QAction * m_actionSketch, * m_actionEraser, * m_actionSelect
            // New Canvas sub-menu
            , * m_actionCanvasClone, * m_actionCanvasXY, * m_actionCanvasYZ, * m_actionCanvasXZ
            , * m_actionCanvasOrtho, * m_actionCanvasSeparate
            // New Canvas Set sub-menu
            , * m_actionSetStandard
//            , * m_actionSetParallel, * m_actionSetCoaxial, * m_actionSetRing
            // Edit Canvas
            , * m_actionCanvasEdit
            // Edit Strokes
            , * m_actionStrokesPush
    ;

    CameraProperties* m_cameraProperties;
};

#endif // MAINWINDOW

