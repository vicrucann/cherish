#ifndef CANVASPHOTOWIDGETTEST_H
#define CANVASPHOTOWIDGETTEST_H

#include "BaseGuiTest.h"

class CanvasPhotoWidgetTest  : public BaseGuiTest
{
Q_OBJECT
private slots:
    void testAddDeleteCanvas();
    void testAddDeletePhoto();
    void testSetCanvasCurrent();
    void testSetCanvasPrevious();
    void testSetPhotoTransparency();
};

#endif // CANVASPHOTOWIDGETTEST_H
