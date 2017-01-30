#ifndef ACTIONS_H
#define ACTIONS_H

#include <QAction>
#include <QObject>
#include <QString>

class BookmarkAction : public QAction
{
    Q_OBJECT
public:
    BookmarkAction(const QString& name, QObject* parent);

public slots:
    void onTriggered();

signals:
    void triggeredName(const QString& name);
};

#endif // ACTIONS_H
