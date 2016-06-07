#ifndef SCENESTATETEST_H
#define SCENESTATETEST_H

#include <QObject>
#include <QTest>

#include "BaseGuiTest.h"

/*! \class SceneStateTest
 * \brief Class that performs testing of SceneState class in relation to the GUI widgets and
 * scene graph.
*/
class SceneStateTest : public BaseGuiTest
{
    Q_OBJECT
private slots:
    void testBasicApi();
    void testBookmarkTaken();
    void testBookmarkClickedOn();

};

#endif // SCENESTATETEST_H
