#ifndef DATA_H
#define DATA_H

#include <QIcon>
#include <QPixmap>

class QIcon;
class QPixmap;

/*! \class Data
 * \brief Class that contains access to cherish resources such as action and GUI icons.
*/
class Data
{
public:
    static const QIcon& appIcon();
    static const QIcon& fileNewSceneIcon();
    static const QIcon& fileCloseIcon();
    static const QIcon& fileExitIcon();
    static const QIcon& fileImageIcon();
    static const QIcon& fileOpenIcon();
    static const QIcon& fileSaveIcon();
    static const QIcon& fileExportIcon();


    static const QIcon& editUndoIcon();
    static const QIcon& editRedoIcon();
    static const QIcon& editCutIcon();
    static const QIcon& editCopyIcon();
    static const QIcon& editPasteIcon();
    static const QIcon& editDeleteIcon();
    static const QIcon& editSettingsIcon();


    static const QIcon& sceneSelectIcon();
    static const QIcon& sceneSelect3DIcon();
    static const QIcon& scenePolygonIcon();
    static const QIcon& sceneSketchIcon();
    static const QIcon& sceneEraserIcon();
    static const QIcon& sceneOrbitIcon();
    static const QIcon& scenePanIcon();
    static const QIcon& sceneZoomIcon();


    static const QIcon& optionsVisibilityIcon();


    static const QIcon& sceneNewCanvasIcon();
    static const QIcon& sceneNewCanvasCloneIcon();
    static const QIcon& sceneNewCanvasXYIcon();
    static const QIcon& sceneNewCanvasYZIcon();
    static const QIcon& sceneNewCanvasXZIcon();
    static const QIcon& sceneNewCanvasOrthoIcon();
    static const QIcon& sceneNewCanvasSeparateIcon();

    static const QIcon& sceneNewCanvasSetIcon();
    static const QIcon& sceneNewCanvasSetParallelIcon();
    static const QIcon& sceneNewCanvasSetCoaxialIcon();
    static const QIcon& sceneNewCanvasSetRingIcon();
    static const QIcon& sceneNewCanvasSetStandardIcon();

    static const QIcon& scenePushStrokesIcon();
    static const QIcon& scenePushImagesIcon();

    static const QIcon& sceneCanvasEditIcon();
    static const QIcon& sceneCanvasOffsetIcon();
    static const QIcon& sceneCanvasRotateIcon();
    static const QIcon& sceneImageMoveIcon();
    static const QIcon& sceneImageRotateIcon();
    static const QIcon& sceneImageScaleIcon();
    static const QIcon& sceneImageFlipVIcon();
    static const QIcon& sceneImageFlipHIcon();
    static const QIcon& sceneImagePushIcon();
    static const QIcon& sceneImageTransparencyOnIcon();
    static const QIcon& sceneImageTransparencyOffIcon();

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
    static const QIcon& viewerBookmarkNewIcon();
    static const QIcon& viewerBookmarkSketchIcon();
    static const QIcon& viewerBookmarkEditIcon();
    static const QIcon& viewerRightIcon();
    static const QIcon& viewerTopIcon();
    static const QIcon& viewerTwoscreenIcon();
    static const QIcon& viewerVirtualIcon();
    static const QIcon& viewerAllCanvas();


    static const QIcon& controlBookmarksIcon();
    static const QIcon& controlCanvasesIcon();
    static const QIcon& controlMoveIcon();
    static const QIcon& controlCanvasVisibilityIcon();
    static const QIcon& controlImagesIcon();
    static const QIcon& controlBookmarkStateIcon();


    static const QIcon& cameraApertureIcon();

    /* Cursors */

    static const QPixmap&  editDeleteCursor();

    static const QPixmap& sceneOrbitPixmap();
    static const QPixmap& scenePanPixmap();
    static const QPixmap& sceneZoomPixmap();

    static const QPixmap& sceneCanvasOffsetCursor();
    static const QPixmap& sceneCanvasRotateCursor();
    static const QPixmap& sceneCanvasCloneCursor();

    static const QPixmap& sceneSelectPixmap();
    static const QPixmap& sceneSelect3DPixmap();
    static const QPixmap& sceneSketchPixmap();
    static const QPixmap& scenePolygonPixmap();
    static const QPixmap& sceneEraserPixmap();
    static const QPixmap& sceneImageFlipHPixmap();
    static const QPixmap& sceneImageFlipVPixmap();
    static const QPixmap& sceneImageMovePixmap();
    static const QPixmap& sceneImageScalePixmap();
    static const QPixmap& sceneImageRotatePixmap();

private:

};

#endif // DATA_H
