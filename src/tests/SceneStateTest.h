#ifndef SCENESTATETEST_H
#define SCENESTATETEST_H

#include <QObject>
#include <QTest>
#include <osg/ref_ptr>
#include "SceneState.h"

/*! \class SceneStateTest
 * \brief Class that performs testing of SceneState class in relation to the GUI widgets and
 * scene graph.
*/
class SceneStateTest : public QObject
{
    Q_OBJECT
private slots:

    void trialTest();

private:

};

#endif // SCENESTATETEST_H
