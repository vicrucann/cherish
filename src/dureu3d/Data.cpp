#include "Data.h"

#include <QIcon>
#include <QPixmap>

const QIcon &Data::fileNewSceneIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/file-new-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/file-new-3x.png"));
    return icon;
}

const QIcon &Data::fileCloseIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/file-close-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/file-close-3x.png"));
    return icon;
}

const QIcon &Data::fileExitIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/file-exit-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/file-exit-3x.png"));
    return icon;
}

const QIcon &Data::fileImageIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/file-image-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/file-image-3x.png"));
    return icon;
}

const QIcon &Data::fileOpenIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/file-open-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/file-open-3x.png"));
    return icon;
}

const QIcon &Data::fileSaveIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/file-save-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/file-save-3x.png"));
    return icon;
}

const QIcon &Data::editUndoIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/edit-undo-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/edit-undo-3x.png"));
    return icon;
}

const QIcon &Data::editRedoIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/edit-redo-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/edit-redo-3x.png"));
    return icon;
}

const QIcon &Data::editCutIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/edit-cut-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/edit-cut-3x.png"));
    return icon;
}

const QIcon &Data::editCopyIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/edit-copy-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/edit-copy-3x.png"));
    return icon;
}

const QIcon &Data::editPasteIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/edit-paste-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/edit-paste-3x.png"));
    return icon;
}

const QIcon &Data::sceneSketchIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-sketch-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-sketch-3x.png"));
    return icon;
}

const QIcon &Data::sceneEraserIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-eraser-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-eraser-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-newcanvas-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-newcanvas-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasCloneIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-newcanvas-clone-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-newcanvas-clone-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasXYIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-newcanvas-xy-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-newcanvas-xy-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasYZIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-newcanvas-yz-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-newcanvas-yz-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasXZIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-newcanvas-xz-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-newcanvas-xz-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasSetParallelIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-newcanvas-parallel-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-newcanvas-parallel-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasSetCoaxialIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-newcanvas-coaxial-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-newcanvas-coaxial-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewCanvasSetRingIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-newcanvas-ring-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-newcanvas-ring-3x.png"));
    return icon;
}

const QIcon &Data::scenePushStrokesIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-push-strokes-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-push-strokes-3x.png"));
    return icon;
}

const QIcon &Data::scenePushImagesIcon()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-push-images-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-push-images-3x.png"));
    return icon;
}

const QIcon &Data::sceneEditCanvasOffset()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-canvas-offset-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-canvas-offset-3x.png"));
    return icon;
}

const QIcon &Data::sceneEditCanvasRotate()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-canvas-rotate-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-canvas-rotate-3x.png"));
    return icon;
}

const QIcon &Data::sceneEditImageMove()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-image-move-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-image-move-3x.png"));
    return icon;
}

const QIcon &Data::sceneEditImageRotate()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-image-rotate-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-image-rotate-3x.png"));
    return icon;
}

const QIcon &Data::sceneEditImageScale()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-image-scale-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-image-scale-3x.png"));
    return icon;
}

const QIcon &Data::sceneEditImageFlip()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-image-flip-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-image-flip-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewGeomRectangle()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-geom-rectangle-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-geom-rectangle-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewGeomArc()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-geom-arc-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-geom-arc-3x.png"));
    return icon;
}

const QIcon &Data::sceneNewGeomPolyline()
{
    Q_ASSERT_X(!QPixmap(":/Actions/scene-geom-poly-3x.png").isNull(), Q_FUNC_INFO, "Required resource not available");
    static QIcon icon(QPixmap(":/Actions/scene-geom-poly-3x.png"));
    return icon;
}

