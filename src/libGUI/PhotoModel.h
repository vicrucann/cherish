#ifndef PHOTOMODEL_H
#define PHOTOMODEL_H

#include <QString>
#include <QStandardItemModel>
#include <QFileSystemModel>
#include <QModelIndex>

class PhotoModel : public QStandardItemModel
{
public:
    PhotoModel();

    void setRootPath(const QString& directory);
    const QString& getRootPath() const;

private:
    QString m_directory;
};

#endif // PHOTOMODEL_H
