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
#include <QColorDialog>
#include <QAction>
#include <QObject>
#include <QString>

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

class BookmarkAction : public QAction
{
    Q_OBJECT
public:
    BookmarkAction(const QString& name, QObject* parent)
        : QAction(name, parent)
    {
        this->connect(this, SIGNAL(triggered(bool)), this, SLOT(onTriggered()));
    }

public slots:
    void onTriggered()
    {
        emit triggeredName(this->text());
    }

signals:
    void triggeredName(const QString& name);
};


/*! \class MainWindow
 * \brief Re-defined QMainWindow that contains all the GUI elements such as GLWidget, menu bars, tool bars and other widgets.
*/
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /*! Constructor: initialies the GUI elements and scene graph structure. */
    explicit MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);

    /*! Destructor: annulates singlton-related pointer. */
    ~MainWindow();

    /*! Singleton design pattern. */
    static MainWindow& instance();

    /*! \return const pointer to RootScene. */
    const RootScene* getRootScene() const;

    /*! A method to obtain pointer on current canvas, if there is one present. */
    entity::Canvas* getCanvasCurrent() const;

    /*! \return pointer on a camera used within GLWidget. */
    osg::Camera* getCamera() const;

    /*! A method to setup camera position and parameters within GLWidget.
     * \param fov2 is the full angle (not half angle) FOV, in degrees. */
    void setCameraView(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up, const double& fov2);

    /*! \return state of the button responsible for whether fog factor is on (true) or off (false). */
    bool getStrokeFogFactor() const;

    /*! A method to obtain a scene screenshot with the given camera position. */
    QPixmap getScreenshot(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);

    /*! \return width of GLWidget. */
    int getViewportWidth() const;

    /*! \return height of GLWidget. */
    int getViewportHeight() const;

    /*! A method to obtain current color of the color dialog (for polygon drawing). */
    osg::Vec4f getCurrentColor() const;

    /*! A method to obtain current camera's FOV.
     * \return full angle FOV (not half angle). */
    double getFOV2() const;

    /*! A method to obtain a pointer on bookmark widget. */
    BookmarkWidget* getBookmarkWidget();

public slots:
    /*! Slot called whenver CherishApplication catches change of tablet proximity. */
    void onSetTabletActivity(bool active);

    /*! Slot called whenver we need to update GLWidget, e.g., when scene graph is changed. */
    void onRequestUpdate();

    /*! Slot called when user performs switch of mouse mode automatically, e.g. by doing double-click. */
    void onAutoSwitchMode(cher::MOUSE_MODE mode);

    /*! Slot called when a bookmark was added to scene graph. */
    void onRequestBookmarkSet(int row);

    /*! Slot called when user requested to delete bookmark from the BookmarkWidget. */
    void onDeleteBookmark(const QModelIndex &index);

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

    /*! Slot called when mouse mode is changed. */
    void onMouseModeSet(cher::MOUSE_MODE mode);

    /*! Slot called when photo's transparency was changed by user from CanvasPhotoWidget. */
    void onPhotoTransparencyPlus(const QModelIndex& index);

    /*! Slot called when photo's transparency was changed by user from CanvasPhotoWidget. */
    void onPhotoTransparencyMinus(const QModelIndex& index);

    /*! Slot called when push photo is performed. */
    void onPhotoPushed(int parent, int start, int, int destination, int);

    /*! Slot called so that to request a data of SceneState, e.g. when setting up a bookmark. */
    void onRequestSceneData(entity::SceneState* state);

    /*! Slot called when scene state change was requested, e.g. when setting up a bookmark. */
    void onRequestSceneStateSet(entity::SceneState* state);

    /*! Slot called to obtain status of global tools visibilities. */
    void onRequestSceneToolStatus(bool& visibility);

    /*! Slot called when user performed drag-and-drop from PhotoWidget to GLWidget of a entity::Photo. */
    void onImportPhoto(const QString& path, const QString& fileName);

    /*! Slot called when bookmark position is finished editing. The slot requests from user whether new canvas should be
     * created within the FOV of the camera position.
     * \param eye is eye position of the camera,
     * \param center is the center position of camera,
     * \param up is up vector of camera position.  */
    void onRequestCanvasCreate(const osg::Vec3f& eye, const osg::Vec3f& center, const osg::Vec3f& up);

    /*! Slot is called whenever user right clicks on Canvas within canvas widget. Called after similar slot
     * of user scene. This slot is only for photo re-scaling mouse mode. */
    void onCanvasClicked(const QModelIndex& index);

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
    void onHomeView();
    void onViewAllCanvas();

    void onSelect();
    void onSelect3d();
    void onErase();
    void onDelete();
    void onSketch();
    void onPolygon();
    void onLineSegment();

    void onNewCanvasClone();
    void onNewCanvasXY();
    void onNewCanvasYZ();
    void onNewCanvasZY();
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
    void onBookmarkNew();
    void onBookmarkSketch();
    void onBookmarkEdit(const QString& name);

    void onStrokeFogFactor();

protected:
    void        initializeActions();
    void        initializeMenus();
    void        initializeToolbars();
    void        initializeCallbacks(); // for ctor
    bool        loadSceneFromFile();

    bool        importPhoto(QString& fileName);
    void        setSceneState(const entity::SceneState* state);

    /*! UI Method to add name of editable bookmark into bookmark's submenu. Has to be run
     * whenever the new bookmark is added throught the SVM method, or when opening new file.
     * Has to be cleared when the scene is cleared, or certain elements deleted when the corresponding
     * bookmark is deleted. */
    void        addMenuBookmark(const QString& name);

    QMdiArea*       m_mdiArea;

    QTabWidget*         m_tabWidget;
    BookmarkWidget*     m_bookmarkWidget;
    CanvasPhotoWidget*  m_canvasWidget;
    PhotoWidget*        m_photoWidget;
    PhotoModel*         m_photoModel;


    // http://doc.qt.io/qt-5/qtwidgets-tools-undoframework-example.html
    QUndoStack*     m_undoStack;

    QMenuBar*       m_menuBar;
    QMenu*          m_submenuBookmarks;
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
            , * m_actionPrevView, * m_actionNextView, * m_actionBookmark/*, * m_actionBookmarkNew*/
            , * m_actionBookmarkSketch , * m_actionCameraSettings, * m_actionHomeView, * m_actionViewAllCanvas;

    // SCENE actions
    QAction * m_actionSketch, * m_actionEraser, * m_actionSelect, * m_actionSelect3d, * m_actionPolygon
            , * m_actionLinesegment
            // New Canvas sub-menu
            , * m_actionCanvasClone, * m_actionCanvasXY, * m_actionCanvasYZ, * m_actionCanvasZY, * m_actionCanvasXZ
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

    CameraProperties*   m_cameraProperties;

    QColorDialog*       m_colorDialog;

    static MainWindow* m_instance;
};

#endif // MAINWINDOW

