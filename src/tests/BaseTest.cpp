#include "BaseTest.h"

BaseTest::BaseTest(QObject *parent) : QObject(parent)
{
}

QTEST_MAIN(BaseTest)
#include "BaseTest.moc"
