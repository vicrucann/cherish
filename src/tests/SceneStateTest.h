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

    /*! Battery of tests to check reading and writing of bookmakrs and their corresponding scene states to file */
    void testReadWrite();

    /*! Battery of tests to check: addition of canvas - AddCanvasCommand */
    void testAddCanvas();

    /*! Battery of tests to check: addition of photo - AddPhotoCommand */
    void testAddPhoto();

    /*! Battery of tests to check: deletion of canvas - EditCanvasDeleteCommand */
    void testDeleteCanvas();

    /*! Battery of tests to check: deletion of photo - EditPhotoDeleteCommand */
    void testDeletePhoto();

};

#endif // SCENESTATETEST_H
