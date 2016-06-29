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
    /*! Battery of tests to check basic API functionality */
    void testBasicApi();

    /*! Battery of tests to check saving of bookmark and its corresponding scene states to scene graph */
    void testBookmarkTaken();

    /*! Battery of tests to check clicking on bookmakrs from GUI and its application to the scene graph */
    void testBookmarkClickedOn();

    /*! Battery of tests to check reading and writing of bookmakrs and their corresponding scene states to file */
    void testReadWrite();

    /*! Battery of tests to check: addition of canvas - fur::AddCanvasCommand */
    void testAddCanvas();

    /*! Battery of tests to check: addition of photo - fur::AddPhotoCommand */
    void testAddPhoto();

    /*! Battery of tests to check: deletion of canvas - fur::EditCanvasDeleteCommand */
    void testDeleteCanvas();

    /*! Battery of tests to check: deletion of photo - fur::EditPhotoDeleteCommand */
    void testDeletePhoto();

};

#endif // SCENESTATETEST_H
