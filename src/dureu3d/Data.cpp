#include "Data.h"

#include <QIcon>
#include <QPixmap>
#include <QString>

const QIcon &Data::fileNewSceneIcon()
{
    return m_fileNewScene;
}

const QIcon &Data::fileCloseIcon()
{
    return m_fileClose;
}

const QIcon &Data::fileExitIcon()
{
    return m_fileExit;
}

const QIcon &Data::fileImageIcon()
{
    return m_fileImage;
}

const QIcon &Data::fileOpenIcon()
{
    return m_fileOpen;
}

const QIcon &Data::fileSaveIcon()
{
    return m_fileSave;
}

const QIcon &Data::editUndoIcon()
{
    return m_editUndo;
}

const QIcon &Data::editRedoIcon()
{
    return m_editRedo;
}

const QIcon &Data::editCutIcon()
{
    return m_editCut;
}

const QIcon &Data::editCopyIcon()
{
    return m_editCopy;
}

const QIcon &Data::editPasteIcon()
{
    return m_editPaste;
}

const QIcon &Data::editDeleteIcon()
{
    return m_editDelete;
}

const QIcon &Data::sceneSelectIcon()
{
   return m_sceneSelect;
}

const QIcon &Data::sceneSketchIcon()
{
    return m_sceneSketch;
}

const QIcon &Data::sceneEraserIcon()
{
    return m_sceneEraser;
}

const QIcon &Data::sceneOrbitIcon()
{
    return m_sceneOrbit;
}

const QIcon &Data::scenePanIcon()
{
    return m_scenePan;
}

const QIcon &Data::sceneZoomIcon()
{
    return m_sceneZoom;
}

const QIcon &Data::sceneNewCanvasIcon()
{
    return m_sceneNewCanvas;
}

const QIcon &Data::sceneNewCanvasCloneIcon()
{
    return m_sceneNewCanvasClone;
}

const QIcon &Data::sceneNewCanvasXYIcon()
{
    return m_sceneNewCanvasXY;
}

const QIcon &Data::sceneNewCanvasYZIcon()
{
    return m_sceneNewCanvasYZ;
}

const QIcon &Data::sceneNewCanvasXZIcon()
{
    return m_sceneNewCanvasXZ;
}

const QIcon &Data::sceneNewCanvasSetIcon()
{
    return m_sceneNewCanvasSet;
}

const QIcon &Data::sceneNewCanvasSetParallelIcon()
{
    return m_sceneNewCanvasSet;
}

const QIcon &Data::sceneNewCanvasSetCoaxialIcon()
{
    return m_sceneNewCanvasSetCoaxial;
}

const QIcon &Data::sceneNewCanvasSetRingIcon()
{
    return m_sceneNewCanvasSetRing;
}

const QIcon &Data::sceneNewCanvasSetStandardIcon()
{
    return m_sceneNewCanvasSetStandard;
}

const QIcon &Data::scenePushStrokesIcon()
{
    return m_scenePushStrokes;
}

const QIcon &Data::scenePushImagesIcon()
{
    return m_scenePushImages;
}

const QIcon &Data::sceneCanvasOffsetIcon()
{
    return m_sceneCanvasOffset;
}

const QIcon &Data::sceneCanvasRotateIcon()
{
    return m_sceneCanvasRotate;
}

const QIcon &Data::sceneImageMoveIcon()
{
    return m_sceneImageMove;
}

const QIcon &Data::sceneImageRotateIcon()
{
    return m_sceneImageRotate;
}

const QIcon &Data::sceneImageScaleIcon()
{
    return m_sceneImageScale;
}

const QIcon &Data::sceneImageFlipVIcon()
{
    return m_sceneImageFlipV;
}

const QIcon &Data::sceneImageFlipHIcon()
{
    return m_sceneImageFlipH;
}

const QIcon &Data::sceneImagePushIcon()
{
    return m_scenePushImages;
}

const QIcon &Data::sceneRectangleIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-rectangle-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-rectangle-24px.svg"));
    return icon;
}

const QIcon &Data::sceneArcIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-arc-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-arc-24px.svg"));
    return icon;
}

const QIcon &Data::scenePolylineIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-polyline-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-polyline-24px.svg"));
    return icon;
}

const QIcon &Data::viewerBackIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-back-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-back-24px.svg"));
    return icon;
}

const QIcon &Data::viewerBottomIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-bottom-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-bottom-24px.svg"));
    return icon;
}

const QIcon &Data::viewerFrontIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-front-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-front-24px.svg"));
    return icon;
}

const QIcon &Data::viewerFullscreenIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-fullscreen-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-fullscreen-24px.svg"));
    return icon;
}

const QIcon &Data::viewerHomeIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-home-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-home-24px.svg"));
    return icon;
}

const QIcon &Data::viewerIsoIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-iso-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-iso-24px.svg"));
    return icon;
}

const QIcon &Data::viewerLeftIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-left-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-left-24px.svg"));
    return icon;
}

const QIcon &Data::viewerNextIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-next-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-next-24px.svg"));
    return icon;
}

const QIcon &Data::viewerPreviousIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-previous-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-previous-24px.svg"));
    return icon;
}

const QIcon &Data::viewerBookmarkIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-bookmark-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-bookmark-24px.svg"));
    return icon;
}

const QIcon &Data::viewerRightIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-right-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-right-24px.svg"));
    return icon;
}

const QIcon &Data::viewerTopIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-top-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-top-24px.svg"));
    return icon;
}

const QIcon &Data::viewerTwoscreenIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-twoscreen-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-twoscreen-24px.svg"));
    return icon;
}

const QIcon &Data::viewerVirtualIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-virtualscreen-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/viewer-virtualscreen-24px.svg"));
    return icon;
}

const QPixmap &Data::sceneOrbitPixmap()
{
    static QPixmap pmap(":/scene-orbit-24px.svg");
    return pmap;
}

const QPixmap &Data::scenePanPixmap()
{
    static QPixmap pmap(":/scene-pan-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneZoomPixmap()
{
    static QPixmap pmap(":/scene-zoom-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneSelectPixmap()
{
    static QPixmap pmap(":/scene-select-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneSketchPixmap()
{
    static QPixmap pmap(":/scene-sketch-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneEraserPixmap()
{
    static QPixmap pmap(":/scene-eraser-cursor-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneImageFlipHPixmap()
{
    static QPixmap pmap(":/scene-image-flipv-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneImageFlipVPixmap()
{
    static QPixmap pmap(":/scene-image-fliph-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneImageMovePixmap()
{
    static QPixmap pmap(":/scene-image-move-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneImageScalePixmap()
{
    static QPixmap pmap(":/scene-image-scale-cursor-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneImageRotatePixmap()
{
    static QPixmap pmap(":/scene-image-rotate-24px.svg");
    return pmap;
}

