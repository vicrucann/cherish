#include <QTest>

class Foo: public QObject{
    Q_OBJECT
private slots:
    void test() {QVERIFY(true); }
};

QTEST_MAIN(Foo)
#include "foo.moc"
