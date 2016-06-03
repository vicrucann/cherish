#include "BaseGuiTest.h"
#include "RootScene.h"
#include "GLWidget.h"
#include "RootScene.h"

BaseGuiTest::BaseGuiTest(QWidget *parent)
    : MainWindow(parent)
{
}

void BaseGuiTest::init()
{
    this->onNewCanvasStandard();
}

void BaseGuiTest::cleanup()
{
    this->getRootScene()->setSavedToFile(true);
    this->onFileNew();
}
