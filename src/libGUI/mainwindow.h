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
    GLWidget* createViewer(Qt::WindowFlags f = 0, int viewmode = 1);

    QDesktopWidget* m_desktop;
    QMdiArea*       m_mdiArea;

    // http://doc.qt.io/qt-5/qtwidgets-tools-undoframework-example.html
    QUndoStack*     m_undoStack;
    QUndoView*      m_undoView;

    QMenuBar*       m_menuBar;
    osg::ref_ptr<RootScene> m_rootScene; // main scene graph

    QAction* m_actionNewFile;
    QAction* m_actionClose;
    QAction* m_actionExit;
    QAction* m_actionImportImage;
    QAction* m_actionOpenFile;
    QAction* m_actionSaveFile;

    void createActions();
    QAction* m_ActionUndo;
    QAction* m_ActionRedo;

    QAction* _mActionSketch;

    QAction* _mActionOrbit;
    QAction* _mActionPan;
    QAction* _mActionZoom;

    QAction* _mActionEraser;
    QAction* _mActionDelete;
    QAction* _mActionMove; //toolMove; // move current photo within current canvas
    QAction* _mActionPushStrokes;
    QAction* _mActionRotate;// toolRotate;
    QAction* _mActionRotateImage;
    QAction* _mActionOffset; // offset current canvas along its normal


    QAction* _mActionSelect;
    QAction *loadImageAction;
    // QToolButton *select;
    // QWidgetAction* selectAction;
};

#endif // MAINWINDOW

