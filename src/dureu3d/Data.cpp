#include "Data.h"

#include <QIcon>
#include <QPixmap>
#include <QString>

const QIcon &Data::appIcon()
{
    static QIcon icon(":/logo.png");
    return icon;
}

const QIcon &Data::fileNewSceneIcon()
{
    Q_ASSERT_X(!QPixmap(":/file-newscene-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/file-newscene-24px.svg");
    return icon;
}

const QIcon &Data::fileCloseIcon()
{
    Q_ASSERT_X(!QPixmap(":file-close-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/file-close-24px.svg");
    return icon;
}

const QIcon &Data::fileExitIcon()
{
    Q_ASSERT_X(!QPixmap(":/file-exit-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/file-exit-24px.svg");
    return icon;
}

const QIcon &Data::fileImageIcon()
{
    Q_ASSERT_X(!QPixmap(":/file-image-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/file-image-24px.svg");
    return icon;
}

const QIcon &Data::fileOpenIcon()
{
    Q_ASSERT_X(!QPixmap(":/file-open-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/file-open-24px.svg");
    return icon;
}

const QIcon &Data::fileSaveIcon()
{
    Q_ASSERT_X(!QPixmap(":/file-save-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/file-save-24px.svg");
    return icon;
}

const QIcon &Data::fileExportIcon()
{
    Q_ASSERT_X(!QPixmap(":/file-export-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/file-export-24px.svg");
    return icon;
}

const QIcon &Data::editUndoIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-undo-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/edit-undo-24px.svg");
    return icon;
}

const QIcon &Data::editRedoIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-redo-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/edit-redo-24px.svg");
    return icon;
}

const QIcon &Data::editCutIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-cut-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/edit-cut-24px.svg");
    return icon;
}

const QIcon &Data::editCopyIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-copy-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/edit-copy-24px.svg");
    return icon;
}

const QIcon &Data::editPasteIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-paste-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/edit-paste-24px.svg");
    return icon;
}

const QIcon &Data::editDeleteIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-delete-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/edit-delete-24px.svg");
    return icon;
}

const QIcon &Data::editSettingsIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-settings-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/edit-settings-24px.svg");
    return icon;
}

const QIcon &Data::sceneSelectIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-select-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-select-24px.svg");
    return icon;
}

const QIcon &Data::sceneSketchIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-sketch-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-sketch-24px.svg");
    return icon;
}

const QIcon &Data::sceneEraserIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-eraser-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-eraser-24px.svg");
    return icon;
}

const QIcon &Data::sceneOrbitIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-orbit-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-orbit-24px.svg");
    return icon;
}

const QIcon &Data::scenePanIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-pan-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-pan-24px.svg");
    return icon;
}

const QIcon &Data::sceneZoomIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-zoom-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-zoom-24px.svg");
    return icon;
}

const QIcon &Data::sceneNewCanvasIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-newcanvas-24px.svg");
    return icon;
}

const QIcon &Data::sceneNewCanvasCloneIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-clone-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-newcanvas-clone-24px.svg");
    return icon;
}

const QIcon &Data::sceneNewCanvasXYIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-xy-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-newcanvas-xy-24px.svg");
    return icon;
}

const QIcon &Data::sceneNewCanvasYZIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-yz-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-newcanvas-yz-24px.svg");
    return icon;
}

const QIcon &Data::sceneNewCanvasXZIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-xz-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-newcanvas-xz-24px.svg");
    return icon;
}

const QIcon &Data::sceneNewCanvasOrthoIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-ortho-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-newcanvas-ortho-24px.svg");
    return icon;
}

const QIcon &Data::sceneNewCanvasSetIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-set-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-newcanvas-set-24px.svg");
    return icon;
}

const QIcon &Data::sceneNewCanvasSetParallelIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-parallel-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-newcanvas-parallel-24px.svg");
    return icon;
}

const QIcon &Data::sceneNewCanvasSetCoaxialIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-coaxial-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-newcanvas-coaxial-24px.svg");
    return icon;
}

const QIcon &Data::sceneNewCanvasSetRingIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-ring-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-newcanvas-ring-24px.svg");
    return icon;
}

const QIcon &Data::sceneNewCanvasSetStandardIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-standard-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-newcanvas-standard-24px.svg");
    return icon;
}

const QIcon &Data::scenePushStrokesIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-push-strokes-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-push-strokes-24px.svg");
    return icon;
}

const QIcon &Data::scenePushImagesIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-image-push-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-image-push-24px.svg");
    return icon;
}

const QIcon &Data::sceneCanvasEditIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-canvas-edit-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-canvas-edit-24px.svg");
    return icon;
}

const QIcon &Data::sceneCanvasOffsetIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-canvas-offset-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-canvas-offset-24px.svg");
    return icon;
}

const QIcon &Data::sceneCanvasRotateIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-canvas-rotate-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-canvas-rotate-24px.svg");
    return icon;
}

const QIcon &Data::sceneImageMoveIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-image-move-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-image-move-24px.svg");
    return icon;
}

const QIcon &Data::sceneImageRotateIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-image-rotate-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-image-rotate-24px.svg");
    return icon;
}

const QIcon &Data::sceneImageScaleIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-image-scale-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-image-scale-24px.svg");
    return icon;
}

const QIcon &Data::sceneImageFlipVIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-image-fliph-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-image-fliph-24px.svg");
    return icon;
}

const QIcon &Data::sceneImageFlipHIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-image-flipv-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-image-flipv-24px.svg");
    return icon;
}

const QIcon &Data::sceneImagePushIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-image-push-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-image-push-24px.svg");
    return icon;
}

const QIcon &Data::sceneImageTransparencyOnIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-photo-transparency-on-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/edit-photo-transparency-on-24px.svg");
    return icon;
}

const QIcon &Data::sceneImageTransparencyOffIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-photo-transparency-off-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/edit-photo-transparency-off-24px.svg");
    return icon;
}

const QIcon &Data::sceneRectangleIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-rectangle-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-rectangle-24px.svg");
    return icon;
}

const QIcon &Data::sceneArcIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-arc-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-arc-24px.svg");
    return icon;
}

const QIcon &Data::scenePolylineIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-polyline-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/scene-polyline-24px.svg");
    return icon;
}

const QIcon &Data::viewerBackIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-back-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-back-24px.svg");
    return icon;
}

const QIcon &Data::viewerBottomIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-bottom-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-bottom-24px.svg");
    return icon;
}

const QIcon &Data::viewerFrontIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-front-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-front-24px.svg");
    return icon;
}

const QIcon &Data::viewerFullscreenIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-fullscreen-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-fullscreen-24px.svg");
    return icon;
}

const QIcon &Data::viewerHomeIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-home-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-home-24px.svg");
    return icon;
}

const QIcon &Data::viewerIsoIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-iso-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-iso-24px.svg");
    return icon;
}

const QIcon &Data::viewerLeftIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-left-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-left-24px.svg");
    return icon;
}

const QIcon &Data::viewerNextIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-next-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-next-24px.svg");
    return icon;
}

const QIcon &Data::viewerPreviousIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-previous-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-previous-24px.svg");
    return icon;
}

const QIcon &Data::viewerBookmarkIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-bookmark-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-bookmark-24px.svg");
    return icon;
}

const QIcon &Data::viewerRightIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-right-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-right-24px.svg");
    return icon;
}

const QIcon &Data::viewerTopIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-top-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-top-24px.svg");
    return icon;
}

const QIcon &Data::viewerTwoscreenIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-twoscreen-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-twoscreen-24px.svg");
    return icon;
}

const QIcon &Data::viewerVirtualIcon()
{
    Q_ASSERT_X(!QPixmap(":/viewer-virtualscreen-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/viewer-virtualscreen-24px.svg");
    return icon;
}

const QIcon &Data::controlBookmarksIcon()
{
    Q_ASSERT_X(!QPixmap(":/control-bookmarks-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/control-bookmarks-24px.svg");
    return icon;
}

const QIcon &Data::controlCanvasesIcon()
{
    Q_ASSERT_X(!QPixmap(":/control-canvases-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/control-canvases-24px.svg");
    return icon;
}

const QIcon &Data::controlMoveIcon()
{
    Q_ASSERT_X(!QPixmap(":/control-move-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/control-move-24px.svg"));
    return icon;
}

const QIcon &Data::controlCanvasVisibilityIcon()
{
    Q_ASSERT_X(!QPixmap(":/control-canvas-visibility-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    Q_ASSERT_X(!QPixmap(":/control-canvas-visibilityno-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon;
    icon.addPixmap(QPixmap(":/control-canvas-visibility-24px.svg"), QIcon::Normal, QIcon::On);
    icon.addPixmap(QPixmap(":/control-canvas-visibilityno-24px.svg"), QIcon::Normal, QIcon::Off);
    return icon;
}

const QIcon &Data::controlImagesIcon()
{
    Q_ASSERT_X(!QPixmap(":/control-images-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/control-images-24px.svg");
    return icon;
}

const QIcon &Data::cameraApertureIcon()
{
    Q_ASSERT_X(!QPixmap(":/camera-aperture-24px.svg").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(":/camera-aperture-24px.svg");
    return icon;
}

const QPixmap &Data::editDeleteCursor()
{
    static QPixmap pmap(":/edit-delete-cursor-24px.svg");
    return pmap;
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

const QPixmap &Data::sceneCanvasOffsetCursor()
{
    static QPixmap pmap(":/scene-canvas-offset-cursor-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneCanvasRotateCursor()
{
    static QPixmap pmap(":/scene-canvas-rotate-cursor-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneCanvasCloneCursor()
{
    static QPixmap pmap(":/scene-newcanvas-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneSelectPixmap()
{
    static QPixmap pmap(":/scene-select-24px.svg");
    return pmap;
}

const QPixmap &Data::sceneSelect3DPixmap()
{
    static QPixmap pmap(":/scene-select3d-24px.svg");
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
