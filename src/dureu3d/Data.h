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

    static const QIcon& sceneNewCanvasSetIcon();
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
    static const QIcon& sceneImageFlipVIcon();
    static const QIcon& sceneImageFlipHIcon();
    static const QIcon& sceneImagePushIcon();

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

    QIcon m_sceneSelect;

};

#endif // DATA_H
