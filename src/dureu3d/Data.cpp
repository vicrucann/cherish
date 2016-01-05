#include "Data.h"

#include <QIcon>
#include <QPixmap>
#include <QString>

const QIcon &Data::fileNewSceneIcon()
{
    Q_ASSERT_X(!QPixmap(":/file-newscene-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/file-newscene-3x.png"));
    return icon;
}

const QIcon &Data::fileCloseIcon()
{
    Q_ASSERT_X(!QPixmap(":file-close-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/file-close-3x.png"));
    return icon;
}

const QIcon &Data::fileExitIcon()
{
    Q_ASSERT_X(!QPixmap(":/file-exit-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/file-exit-3x.png"));
    return icon;
}

const QIcon &Data::fileImageIcon()
{
    Q_ASSERT_X(!QPixmap(":/file-image-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/file-image-3x.png"));
    return icon;
}

const QIcon &Data::fileOpenIcon()
{
    Q_ASSERT_X(!QPixmap(":/file-open-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/file-open-3x.png"));
    return icon;
}

const QIcon &Data::fileSaveIcon()
{
    Q_ASSERT_X(!QPixmap(":/file-save-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/file-save-3x.png"));
    return icon;
}

const QIcon &Data::editUndoIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-undo-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/edit-undo-3x.png"));
    return icon;
}

const QIcon &Data::editRedoIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-redo-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/edit-redo-3x.png"));
    return icon;
}

const QIcon &Data::editCutIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-cut-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/edit-cut-3x.png"));
    return icon;
}

const QIcon &Data::editCopyIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-copy-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/edit-copy-3x.png"));
    return icon;
}

const QIcon &Data::editPasteIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-paste-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/edit-paste-3x.png"));
    return icon;
}

const QIcon &Data::editDeleteIcon()
{
    Q_ASSERT_X(!QPixmap(":/edit-delete-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/edit-delete-3x.png"));
    return icon;
}

const QIcon &Data::sceneSketchIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-sketch-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-sketch-3x.png"));
    return icon;
}

const QIcon &Data::sceneEraserIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-eraser-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-eraser-3x.png"));
    return icon;
}

const QIcon &Data::sceneOrbitIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-orbit-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-orbit-3x.png"));
    return icon;
}

const QIcon &Data::scenePanIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-pan-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-pan-3x.png"));
    return icon;
}

const QIcon &Data::sceneZoomIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-zoom-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-zoom-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-newcanvas-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasCloneIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-clone-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-newcanvas-clone-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasXYIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-xy-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-newcanvas-xy-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasYZIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-yz-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-newcanvas-yz-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasXZIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-xz-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-newcanvas-xz-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasSetParallelIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-parallel-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-newcanvas-parallel-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasSetCoaxialIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-coaxial-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-newcanvas-coaxial-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasSetRingIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-ring-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-newcanvas-ring-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasSetStandardIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-newcanvas-standard-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-newcanvas-standard-3x.png"));
    return icon;
}

const QIcon &Data::scenePushStrokesIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-push-strokes-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-push-strokes-3x.png"));
    return icon;
}

const QIcon &Data::scenePushImagesIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-push-images-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-push-images-3x.png"));
    return icon;
}

const QIcon &Data::sceneCanvasOffsetIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-canvas-offset-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-canvas-offset-3x.png"));
    return icon;
}

const QIcon &Data::sceneCanvasRotateIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-canvas-rotate-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-canvas-rotate-3x.png"));
    return icon;
}

const QIcon &Data::sceneImageMoveIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-image-move-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-image-move-3x.png"));
    return icon;
}

const QIcon &Data::sceneImageRotateIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-image-rotate-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-image-rotate-3x.png"));
    return icon;
}

const QIcon &Data::sceneImageScaleIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-image-scale-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-image-scale-3x.png"));
    return icon;
}

const QIcon &Data::sceneImageFlipIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-image-flip-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-image-flip-3x.png"));
    return icon;
}

const QIcon &Data::sceneImagePushIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-image-push-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-image-push-3x.png"));
    return icon;
}

const QIcon &Data::sceneRectangleIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-geom-rectangle-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-geom-rectangle-3x.png"));
    return icon;
}

const QIcon &Data::sceneArcIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-geom-arc-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-geom-arc-3x.png"));
    return icon;
}

const QIcon &Data::scenePolylineIcon()
{
    Q_ASSERT_X(!QPixmap(":/scene-geom-poly-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/scene-geom-poly-3x.png"));
    return icon;
}

