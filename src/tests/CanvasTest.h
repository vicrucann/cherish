#ifndef CANVASTEST_H
#define CANVASTEST_H

#include "BaseGuiTest.h"

class CanvasTest : public BaseGuiTest
{
    Q_OBJECT
private slots:
    void testBasicApi();
    void testReadWrite();

    void testNewXY();
    void testNewYZ();
    void testNewXZ();
    void testCloneOrtho();

private:
    bool differenceWithinThreshold(const osg::Vec3f& X, const osg::Vec3f& Y);
    void testOrthogonality(entity::Canvas* canvas);
    void testStructure(entity::Canvas* canvas);

};

#endif // CANVASTEST_H
