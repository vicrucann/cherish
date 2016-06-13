#ifndef USERSCENETEST_H
#define USERSCENETEST_H

#include <QObject>
#include <QTest>

#include "BaseGuiTest.h"

class UserSceneTest : public BaseGuiTest
{
    Q_OBJECT
private slots:

    void testBasicApi();
    void testWriteReadCanvases();
    void testWriteReadBookmarks();

//    void testAddCanvas();
//    void testCurrentPreviousCanvas();
//    void testDeleteCanvas();
//    void testGetCanvas();
//    void testEditCanvas();

//    void testAddStroke();
//    void testEditStroke();

//    void testAddPhoto();
//    void testEditPhoto();
//    void testGetPhoto();

//    void testAddBookmark();
//    void testDeleteBookmark();

private:

};

#endif // USERSCENETEST_H
