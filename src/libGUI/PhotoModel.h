#ifndef PHOTOMODEL_H
#define PHOTOMODEL_H

#include <QString>
#include <QStringList>
#include <QStandardItemModel>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QModelIndexList>

// drag and drop additional info: http://doc.qt.io/qt-5/model-view-programming.html#using-drag-and-drop-with-item-views

class PhotoModel : public QStandardItemModel
{
public:
    PhotoModel();

    void setRootPath(const QString& directory);
    const QString& getRootPath() const;

    virtual Qt::DropActions supportedDragActions() const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QStringList mimeTypes() const;
    virtual QMimeData* mimeData(const QModelIndexList &indexes) const;

private:
    QString m_directory;
};

#endif // PHOTOMODEL_H
