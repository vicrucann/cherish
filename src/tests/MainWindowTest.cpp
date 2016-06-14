#include "MainWindowTest.h"

#include <QTreeWidgetItem>
#include <QSignalSpy>

void MainWindowTest::testToolsOnOff()
{
    /* change current scene state: make canvas0 and canvas2 invisible */
    this->onVisibilitySetCanvas(0);
    this->onVisibilitySetCanvas(2);
    QCOMPARE(m_canvasWidget->topLevelItemCount(), 3);

    QCOMPARE(m_canvas0->getVisibilityAll(), false);
    QCOMPARE(m_canvas0->getVisibilityData(), m_canvas0->getVisibilityFrame());
    QCOMPARE(m_canvas0->getVisibilityFrameInternal(), true);
    QTreeWidgetItem* item0 = m_canvasWidget->topLevelItem(0);
    QVERIFY(item0);
    QCOMPARE(item0->data(0, cher::DelegateVisibilityRole).toBool(), true);

    QCOMPARE(m_canvas1->getVisibilityAll(), true);
    QCOMPARE(m_canvas1->getVisibilityData(), m_canvas1->getVisibilityFrame());
    QCOMPARE(m_canvas1->getVisibilityFrameInternal(), true);
    QTreeWidgetItem* item1 = m_canvasWidget->topLevelItem(1);
    QVERIFY(item1);
    QCOMPARE(item1->data(0, cher::DelegateVisibilityRole).toBool(), false);

    QCOMPARE(m_canvas2->getVisibilityAll(), false);
    QCOMPARE(m_canvas2->getVisibilityData(), m_canvas2->getVisibilityFrame());
    QCOMPARE(m_canvas2->getVisibilityFrameInternal(), true);
    QTreeWidgetItem* item2 = m_canvasWidget->topLevelItem(2);
    QVERIFY(item2);
    QCOMPARE(item2->data(0, cher::DelegateVisibilityRole).toBool(), true);

    QCOMPARE(m_rootScene->getAxesVisibility(), true);
    QCOMPARE(m_rootScene->getBookmarkToolVisibility(), true);
    QCOMPARE(m_actionTools->isChecked(), true);


    /* imitate tools off */
    QSignalSpy spy_tools(m_actionTools, SIGNAL(toggled(bool)));
    m_actionTools->setChecked(false);
    QCOMPARE(spy_tools.count(), 1);
    QList<QVariant> args_tools = spy_tools.takeFirst();
    QVERIFY(args_tools.at(0).toBool() == false);

    QCOMPARE(m_canvas0->getVisibilityAll(), false);
    QCOMPARE(m_canvas0->getVisibilityData(), m_canvas0->getVisibilityFrame());
    QCOMPARE(m_canvas0->getVisibilityFrameInternal(), false);
    QCOMPARE(item0->data(0, cher::DelegateVisibilityRole).toBool(), true);

    QCOMPARE(m_canvas1->getVisibilityAll(), true);
    QCOMPARE(m_canvas1->getVisibilityData(), m_canvas1->getVisibilityFrame());
    QCOMPARE(m_canvas1->getVisibilityFrameInternal(), false);
    QCOMPARE(item1->data(0, cher::DelegateVisibilityRole).toBool(), false);

    QCOMPARE(m_canvas2->getVisibilityAll(), false);
    QCOMPARE(m_canvas2->getVisibilityData(), m_canvas2->getVisibilityFrame());
    QCOMPARE(m_canvas2->getVisibilityFrameInternal(), false);
    QCOMPARE(item2->data(0, cher::DelegateVisibilityRole).toBool(), true);

    QCOMPARE(m_rootScene->getAxesVisibility(), false);
    QCOMPARE(m_rootScene->getBookmarkToolVisibility(), false);
    QCOMPARE(m_actionTools->isChecked(), false);


    /* turn visibility back for canvas0 while tools are off */
    this->onVisibilitySetCanvas(0);
    QCOMPARE(m_canvas0->getVisibilityAll(), true);
    QCOMPARE(m_canvas0->getVisibilityData(), m_canvas0->getVisibilityFrame());
    QCOMPARE(m_canvas0->getVisibilityFrameInternal(), false);
    QCOMPARE(item0->data(0, cher::DelegateVisibilityRole).toBool(), false);


    /* add another canvas */
    QSignalSpy spy_canvas(m_scene.get(), SIGNAL(requestSceneToolStatus(bool&)));
    this->onNewCanvasXY();
    QCOMPARE(spy_canvas.count(), 1);
    QCOMPARE(m_scene->getNumCanvases(), 4);
    entity::Canvas* m_canvas3 = m_scene->getCanvas(3);
    QVERIFY(m_canvas3);
    QTreeWidgetItem* item3 = m_canvasWidget->topLevelItem(3);
    QVERIFY(item3);
    QCOMPARE(m_canvas3->getVisibilityAll(), true);
    QCOMPARE(m_canvas3->getVisibilityData(), m_canvas3->getVisibilityFrame());
    QCOMPARE(m_canvas3->getVisibilityFrameInternal(), false);
    QCOMPARE(item3->data(0, cher::DelegateVisibilityRole).toBool(), false);


    /* imitate tools on */
    m_actionTools->setChecked(true);

    QCOMPARE(m_canvas0->getVisibilityAll(), true);
    QCOMPARE(m_canvas0->getVisibilityData(), m_canvas0->getVisibilityFrame());
    QCOMPARE(m_canvas0->getVisibilityFrameInternal(), true);
    QCOMPARE(item0->data(0, cher::DelegateVisibilityRole).toBool(), false);

    QCOMPARE(m_canvas1->getVisibilityAll(), true);
    QCOMPARE(m_canvas1->getVisibilityData(), m_canvas1->getVisibilityFrame());
    QCOMPARE(m_canvas1->getVisibilityFrameInternal(), true);
    QCOMPARE(item1->data(0, cher::DelegateVisibilityRole).toBool(), false);

    QCOMPARE(m_canvas2->getVisibilityAll(), false);
    QCOMPARE(m_canvas2->getVisibilityData(), m_canvas2->getVisibilityFrame());
    QCOMPARE(m_canvas2->getVisibilityFrameInternal(), true);
    QCOMPARE(item2->data(0, cher::DelegateVisibilityRole).toBool(), true);

    QCOMPARE(m_canvas3->getVisibilityAll(), true);
    QCOMPARE(m_canvas3->getVisibilityData(), m_canvas3->getVisibilityFrame());
    QCOMPARE(m_canvas3->getVisibilityFrameInternal(), true);
    QCOMPARE(item3->data(0, cher::DelegateVisibilityRole).toBool(), false);

    QCOMPARE(m_rootScene->getAxesVisibility(), true);
    QCOMPARE(m_rootScene->getBookmarkToolVisibility(), true);
    QCOMPARE(m_actionTools->isChecked(), true);
}

QTEST_MAIN(MainWindowTest)
#include "MainWindowTest.moc"
