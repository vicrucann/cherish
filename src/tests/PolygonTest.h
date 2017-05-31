#ifndef POLYGONTEST_H
#define POLYGONTEST_H

#include "BaseGuiTest.h"

class PolygonTest : public BaseGuiTest
{
    Q_OBJECT
private slots:
    void testAddPolygon();
    void testCloneShaderedPolygon();
    void testReadWrite();
    void testCopyPaste();
    void testFogSwitch();
};

#endif // POLYGONTEST_H
