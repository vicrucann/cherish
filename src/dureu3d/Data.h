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
    static const QIcon& editDeleteIcon();


    static const QIcon& sceneSketchIcon();
    static const QIcon& sceneEraserIcon();
    static const QIcon& sceneOrbitIcon();
    static const QIcon& scenePanIcon();
    static const QIcon& sceneZoomIcon();

    static const QIcon& sceneNewCanvasIcon();
    static const QIcon& sceneNewCanvasCloneIcon();
    static const QIcon& sceneNewCanvasXYIcon();
    static const QIcon& sceneNewCanvasYZIcon();
    static const QIcon& sceneNewCanvasXZIcon();
    static const QIcon& sceneNewCanvasSetParallelIcon();
    static const QIcon& sceneNewCanvasSetCoaxialIcon();
    static const QIcon& sceneNewCanvasSetRingIcon();
    static const QIcon& sceneNewCanvasSetStandardIcon();

    static const QIcon& scenePushStrokesIcon();
    static const QIcon& scenePushImagesIcon();

    static const QIcon& sceneCanvasOffsetIcon();
    static const QIcon& sceneCanvasRotateIcon();
    static const QIcon& sceneImageMoveIcon();
    static const QIcon& sceneImageRotateIcon();
    static const QIcon& sceneImageScaleIcon();
    static const QIcon& sceneImageFlipIcon();
    static const QIcon& sceneImagePushIcon();

    static const QIcon& sceneRectangleIcon();
    static const QIcon& sceneArcIcon();
    static const QIcon& scenePolylineIcon();

private:

};

#endif // DATA_H
