#include "Actions.h"

BookmarkAction::BookmarkAction(const QString &name, QObject *parent)
    : QAction(name, parent)
{
    this->connect(this, SIGNAL(triggered(bool)), this, SLOT(onTriggered()));
}

void BookmarkAction::onTriggered()
{
    emit triggeredName(this->text());
}
