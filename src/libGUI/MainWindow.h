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
#include <QFileSystemModel>

#include <osg/ref_ptr>
#include <osg/Camera>

#include "RootScene.h"
#include "Settings.h"
#include "GLWidget.h"
#include "CameraProperties.h"
#include "ListWidget.h"
#include "TreeWidget.h"
#include "ListView.h"
#include "PhotoModel.h"
#include "Data.h"

/*! \class MainWindow
 * \brief Re-defined QMainWindow that contains all the GUI elements such as GLWidget, menu bars, tool bars and other widgets.
*/
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /*! Constructor. */
    explicit MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();

    static MainWindow& instance();

    /*! \return const pointer to RootScene. */
    const RootScene* getRootScene() const;

    entity::Canvas* getCanvasCurrent() const;
    osg::Camera* getCamera() const;
    bool getStrokeFogFactor() const;
    QPixmap getScreenshot(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);

    void setMouseMode(cher::MOUSE_MODE mode);
    void doAutoSwitchMode(cher::MOUSE_MODE mode);

    /*! Method called from GLWidget when user performed drag-and-drop from PhotoWidget to GLWidget of a entity::Photo. */
    void doImportPhoto(const QString &path, const QString &fileName);

    /*! Method called whenever we need to update GLWidget, e.g., when scene graph is changed. */
    void doUpdate();

    /*! Method to obtain status of global tools visibilities. */
    bool getSceneToolStatus() const;

    /*! Method called when a bookmark was added to scene graph. */
    void doBookmarkSet(int row);

    /*! Method called when user requested to delete bookmark from the BookmarkWidget. */
    void doDeleteBookmark(const QModelIndex &index);

    /*! Method to request a data of SceneState, e.g. when setting up a bookmark. */
    entity::SceneState* getSceneData();

public slots:
    /*! Slot called whenver CherishApplication catches change of tablet proximity. */
    void onSetTabletActivity(bool active);

    /*! Slot called when user requested to delete canvas from CanvasPhotoWidget. */
    void onDeleteCanvas(const QModelIndex& index);

    /*! Slot called when user requested to delete photo from CanvasPhotoWidget. */
    void onDeletePhoto(const QModelIndex& index);

    /*! Slot called when user requested to set canvas visibility by using controls of CanvasPhotoWidget. */
    void onVisibilitySetCanvas(int index);

    /*! Slot called when user changed order of bookmarks within BookmarkWidget. */
    void onMoveBookmark(const QModelIndex &index);

    /*! Slot called when BookmarkWidget content had changed, i.e., new row was inserted. */
    void onBookmarkAddedToWidget(const QModelIndex &, int first, int last);

    /*! Slot called when BookmarkWidget content had changed, i.e., a row was removed. */
    void onBookmarkRemovedFromWidget(const QModelIndex &, int first, int last);

    /*! Slot called when photo's transparency was changed by user from CanvasPhotoWidget. */
    void onPhotoTransparencyPlus(const QModelIndex& index);

    /*! Slot called when photo's transparency was changed by user from CanvasPhotoWidget. */
    void onPhotoTransparencyMinus(const QModelIndex& index);

    /*! Slot called when push photo is performed. */
    void onPhotoPushed(int parent, int start, int, int destination, int);

    /*! Slot called when scene state change was requested, e.g. when setting up a bookmark. */
    void onRequestSceneStateSet(entity::SceneState* state);

protected slots:
    /* NOTE: there should be no private slots, since all are used for unit tests */
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileExport();
    void onFileImage();
    void onFilePhotoBase();
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

    void onStrokeFogFactor();

protected:
    void        initializeActions();
    void        initializeMenus();
    void        initializeToolbars();
    void        initializeCallbacks(); // for ctor
    bool        loadSceneFromFile();

    bool        importPhoto(QString& fileName);

    QMdiArea*       m_mdiArea;

    QTabWidget*         m_tabWidget;
    BookmarkWidget*     m_bookmarkWidget;
    CanvasPhotoWidget*  m_canvasWidget;
    PhotoWidget*        m_photoWidget;
    PhotoModel*         m_photoModel;


    // http://doc.qt.io/qt-5/qtwidgets-tools-undoframework-example.html
    QUndoStack*     m_undoStack;

    QMenuBar*       m_menuBar;
    osg::ref_ptr<RootScene> m_rootScene; // main scene graph
    QUndoStack* m_viewStack;
    GLWidget*       m_glWidget;

    // FILE actions
    QAction * m_actionNewFile, * m_actionClose, * m_actionExit,
            * m_actionImportImage, * m_actionOpenFile, * m_actionSaveFile,
            * m_actionSaveAsFile, * m_actionExportAs, * m_actionPhotoBase;

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

    // OPTION actions
    QAction* m_actionStrokeFogFactor;

    CameraProperties* m_cameraProperties;

    static MainWindow* m_instance;
};

#endif // MAINWINDOW

