#ifndef DATA_H
#define DATA_H

#include <QIcon>
#include <QPixmap>

class QIcon;
class QPixmap;

class Data
{
public:
    Data()
        : m_fileNewScene(QPixmap(":/file-newscene-24px.svg"))
        , m_fileClose(QPixmap(":file-close-24px.svg"))
        , m_fileExit(QPixmap(":/file-exit-24px.svg"))
        , m_fileImage(QPixmap(":/file-image-24px.svg"))
        , m_fileOpen(QPixmap(":/file-open-24px.svg"))
        , m_fileSave(QPixmap(":/file-save-24px.svg"))

        , m_editUndo(QPixmap(":/edit-undo-24px.svg"))
        , m_editRedo(QPixmap(":/edit-redo-24px.svg"))
        , m_editCut(QPixmap(":/edit-cut-24px.svg"))
        , m_editCopy(QPixmap(":/edit-copy-24px.svg"))
        , m_editPaste(QPixmap(":/edit-paste-24px.svg"))
        , m_editDelete(QPixmap(":/edit-delete-24px.svg"))

        , m_sceneSelect(QPixmap(":/scene-select-24px.svg"))
        , m_sceneSketch(QPixmap(":/scene-sketch-24px.svg"))
        , m_sceneEraser(QPixmap(":/scene-eraser-24px.svg"))
        , m_sceneOrbit(QPixmap(":/scene-orbit-24px.svg"))
        , m_scenePan(QPixmap(":/scene-pan-24px.svg"))
        , m_sceneZoom(QPixmap(":/scene-zoom-24px.svg"))

        , m_sceneNewCanvas(QPixmap(":/scene-newcanvas-24px.svg"))
        , m_sceneNewCanvasClone(QPixmap(":/scene-newcanvas-clone-24px.svg"))
        , m_sceneNewCanvasXY(QPixmap(":/scene-newcanvas-xy-24px.svg"))
        , m_sceneNewCanvasYZ(QPixmap(":/scene-newcanvas-yz-24px.svg"))
        , m_sceneNewCanvasXZ(QPixmap(":/scene-newcanvas-xz-24px.svg"))
        , m_sceneNewCanvasSet(QPixmap(":/scene-newcanvas-set-24px.svg"))
        , m_sceneNewCanvasSetParallel(QPixmap(":/scene-newcanvas-parallel-24px.svg"))
        , m_sceneNewCanvasSetCoaxial((QPixmap(":/scene-newcanvas-coaxial-24px.svg")))
        , m_sceneNewCanvasSetRing((QPixmap(":/scene-newcanvas-ring-24px.svg")))
        , m_sceneNewCanvasSetStandard(QPixmap(":/scene-newcanvas-standard-24px.svg"))

        , m_scenePushStrokes(QPixmap(":/scene-push-strokes-24px.svg"))
        , m_scenePushImages(QPixmap(":/scene-image-push-24px.svg"))
        , m_sceneCanvasOffset(QPixmap(":/scene-canvas-offset-24px.svg"))
        , m_sceneCanvasRotate(QPixmap(":/scene-canvas-rotate-24px.svg"))
        , m_sceneImageMove(QPixmap(":/scene-image-move-24px.svg"))
        , m_sceneImageRotate(QPixmap(":/scene-image-rotate-24px.svg"))
        , m_sceneImageScale(QPixmap(":/scene-image-scale-24px.svg"))
        , m_sceneImageFlipV(QPixmap(":/scene-image-fliph-24px.svg"))
        , m_sceneImageFlipH(QPixmap(":/scene-image-flipv-24px.svg"))
    {
    }

    //QIcon getTestIcon(){ return m_iconTest; }

    const QIcon& fileNewSceneIcon();
    const QIcon& fileCloseIcon();
    const QIcon& fileExitIcon();
    const QIcon& fileImageIcon();
    const QIcon& fileOpenIcon();
    const QIcon& fileSaveIcon();


    const QIcon& editUndoIcon();
    const QIcon& editRedoIcon();
    const QIcon& editCutIcon();
    const QIcon& editCopyIcon();
    const QIcon& editPasteIcon();
    const QIcon& editDeleteIcon();


    const QIcon& sceneSelectIcon();
    const QIcon& sceneSketchIcon();
    const QIcon& sceneEraserIcon();
    const QIcon& sceneOrbitIcon();
    const QIcon& scenePanIcon();
    const QIcon& sceneZoomIcon();

    const QIcon& sceneNewCanvasIcon();
    const QIcon& sceneNewCanvasCloneIcon();
    const QIcon& sceneNewCanvasXYIcon();
    const QIcon& sceneNewCanvasYZIcon();
    const QIcon& sceneNewCanvasXZIcon();

    const QIcon& sceneNewCanvasSetIcon();
    const QIcon& sceneNewCanvasSetParallelIcon();
    const QIcon& sceneNewCanvasSetCoaxialIcon();
    const QIcon& sceneNewCanvasSetRingIcon();
    const QIcon& sceneNewCanvasSetStandardIcon();

    const QIcon& scenePushStrokesIcon();
    const QIcon& scenePushImagesIcon();

    const QIcon& sceneCanvasOffsetIcon();
    const QIcon& sceneCanvasRotateIcon();
    const QIcon& sceneImageMoveIcon();
    const QIcon& sceneImageRotateIcon();
    const QIcon& sceneImageScaleIcon();
    const QIcon& sceneImageFlipVIcon();
    const QIcon& sceneImageFlipHIcon();
    const QIcon& sceneImagePushIcon();

    static const QIcon& sceneRectangleIcon();
    static const QIcon& sceneArcIcon();
    static const QIcon& scenePolylineIcon();

    static const QIcon& viewerBackIcon();
    static const QIcon& viewerBottomIcon();
    static const QIcon& viewerFrontIcon();
    static const QIcon& viewerFullscreenIcon();
    static const QIcon& viewerHomeIcon();
    static const QIcon& viewerIsoIcon();
    static const QIcon& viewerLeftIcon();
    static const QIcon& viewerNextIcon();
    static const QIcon& viewerPreviousIcon();
    static const QIcon& viewerBookmarkIcon();
    static const QIcon& viewerRightIcon();
    static const QIcon& viewerTopIcon();
    static const QIcon& viewerTwoscreenIcon();
    static const QIcon& viewerVirtualIcon();

    /* Cursors */

    static const QPixmap& sceneOrbitPixmap();
    static const QPixmap& scenePanPixmap();
    static const QPixmap& sceneZoomPixmap();

    static const QPixmap& sceneSelectPixmap();
    static const QPixmap& sceneSketchPixmap();
    static const QPixmap& sceneEraserPixmap();
    static const QPixmap& sceneImageFlipHPixmap();
    static const QPixmap& sceneImageFlipVPixmap();
    static const QPixmap& sceneImageMovePixmap();
    static const QPixmap& sceneImageScalePixmap();
    static const QPixmap& sceneImageRotatePixmap();

private:
    QIcon m_fileNewScene, m_fileClose, m_fileExit, m_fileImage,
    m_fileOpen, m_fileSave;

    QIcon m_editUndo, m_editRedo, m_editCut, m_editCopy, m_editPaste,
    m_editDelete;

    QIcon m_sceneSelect, m_sceneSketch, m_sceneEraser, m_sceneOrbit, m_scenePan,
    m_sceneZoom, m_sceneNewCanvas, m_sceneNewCanvasClone, m_sceneNewCanvasXY,
    m_sceneNewCanvasYZ, m_sceneNewCanvasXZ;
    QIcon m_sceneNewCanvasSet, m_sceneNewCanvasSetParallel,
    m_sceneNewCanvasSetCoaxial, m_sceneNewCanvasSetRing,
    m_sceneNewCanvasSetStandard;
    QIcon m_scenePushStrokes, m_scenePushImages, m_sceneCanvasOffset,
    m_sceneCanvasRotate, m_sceneImageMove, m_sceneImageRotate, m_sceneImageScale,
    m_sceneImageFlipV, m_sceneImageFlipH;

};

#endif // DATA_H
