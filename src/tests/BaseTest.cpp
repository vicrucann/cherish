#include "BaseTest.h"
#include "RootScene.h"
#include "GLWidget.h"
#include "RootScene.h"

BaseTest::BaseTest(QWidget *parent)
    : MainWindow(parent)
{
}

void BaseTest::init()
{
    this->onNewCanvasStandard();
}

void BaseTest::cleanup()
{
    this->getRootScene()->setSavedToFile(true);
    this->onFileNew();
}

QTEST_MAIN(BaseTest)
#include "BaseTest.moc"
