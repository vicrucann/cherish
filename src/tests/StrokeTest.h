#ifndef STROKETEST_H
#define STROKETEST_H

#include "BaseGuiTest.h"

class StrokeTest : public BaseGuiTest
{
    Q_OBJECT
private slots:
    void testBasicApi();
    void testReadWrite();

private:

};

#endif // STROKETEST_H
