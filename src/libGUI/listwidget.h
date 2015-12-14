#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QtGui>
#include <QListWidget>
#include <QLabel>
class ListWidget : public QWidget
{
public:
        ListWidget(QWidget * parent = 0);
        ~ListWidget();
private:
        QLabel *label;
        QListWidget *list;
};

#endif // LISTWIDGET_H
