#ifndef TEST_ROOTSCENE
#define TEST_ROOTSCENE

#include <QObject>
#include <QTest>
#include <osg/ref_ptr>

#include "../libSGEntities/rootscene.h"

class RootSceneTest : public QObject {
    Q_OBJECT
public:
    RootSceneTest();
private slots:
    void initialValues();

    void addDeleteCanvas();
    //void addCanvasCopyAs();
    //void deleteCanvas();

    void setNameUserScene();
private:
    osg::ref_ptr<RootScene> _RS;
};

#endif // TEST_ROOTSCENE

