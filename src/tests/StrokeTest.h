#ifndef STROKETEST_H
#define STROKETEST_H

#include "BaseGuiTest.h"

class StrokeTest : public BaseGuiTest
{
    Q_OBJECT
private slots:
    void testAddStroke();
    void testCloneShaderedStroke();
    void testReadWrite();
    void testCopyPaste();
    void testFogSwitch();

private:

};

#endif // STROKETEST_H
