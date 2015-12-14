#include "listwidget.h"
#include <QPropertyAnimation>
#include <QHBoxLayout>
#include <QCursor>

ListWidget::ListWidget(QWidget *parent)
{
        //color box
        QCursor *myCursor=new QCursor(QPixmap(":/images/eraser.png"),-1,-1);
        setCursor(*myCursor);
        // We recommend using 32 x 32 cursors, because this size is supported on all platforms.
        label = new QLabel;
        label->setFixedWidth(80);
        list = new QListWidget(parent);
        list->setViewMode(QListView::IconMode);
        list->setIconSize(QSize(100, 100));
        list->setFlow(QListWidget::LeftToRight);
        list->setWrapping(false);
        list->setMovement(QListView::Static);

        //another way to create an item
        //QListWidgetItem* itemLW = new QListWidgetItem();
        //itemLW->setText("position");
        //itemLW->setIcon(QIcon(":/images/scene.png"));
        //list->addItem(itemLW);
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 1")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 2")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 3")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 4")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 5")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 6")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 7")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 8")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 9")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 10")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 11")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 12")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 13")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 14")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 15")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 16")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 17")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 18")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 19")));
        list->addItem(new QListWidgetItem(QIcon(":/scene.png"), tr("Position 20")));
        list->setMinimumHeight(140);
        list->setMaximumHeight(300);
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(label);
        layout->addWidget(list);

        setLayout(layout);

        connect(list, SIGNAL(currentTextChanged(QString)), label, SLOT(setText(QString)));
}

ListWidget::~ListWidget(){}
