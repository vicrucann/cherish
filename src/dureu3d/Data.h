#ifndef DATA_H
#define DATA_H

#include <QIcon>
#include <QPixmap>

class Data
{
public:
    static const QIcon& fileNewSceneIcon();
    static const QIcon& fileCloseIcon();
    static const QIcon& fileExitIcon();
    static const QIcon& fileImageIcon();
    static const QIcon& fileOpenIcon();
    static const QIcon& fileSaveIcon();


    static const QIcon& editUndoIcon();
    static const QIcon& editRedoIcon();
    static const QIcon& editCutIcon();
    static const QIcon& editCopyIcon();
    static const QIcon& editPasteIcon();


    static const QIcon& sceneSketchIcon();
    static const QIcon& sceneEraserIcon();

    static const QIcon& sceneNewCanvasIcon();
    static const QIcon& sceneNewCanvasCloneIcon();
    static const QIcon& sceneNewCanvasXYIcon();
    static const QIcon& sceneNewCanvasYZIcon();
    static const QIcon& sceneNewCanvasXZIcon();
    static const QIcon& sceneNewCanvasSetParallelIcon();
    static const QIcon& sceneNewCanvasSetCoaxialIcon();
    static const QIcon& sceneNewCanvasSetRingIcon();

    static const QIcon& scenePushStrokesIcon();
    static const QIcon& scenePushImagesIcon();

    static const QIcon& sceneEditCanvasOffset();
    static const QIcon& sceneEditCanvasRotate();
    static const QIcon& sceneEditImageMove();
    static const QIcon& sceneEditImageRotate();
    static const QIcon& sceneEditImageScale();
    static const QIcon& sceneEditImageFlip();

    static const QIcon& sceneNewGeomRectangle();
    static const QIcon& sceneNewGeomArc();
    static const QIcon& sceneNewGeomPolyline();

private:

};

#endif // DATA_H
