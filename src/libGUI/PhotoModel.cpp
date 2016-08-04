#include "PhotoModel.h"

#include <QDir>
#include <QFileInfoList>
#include <QPixmap>
#include <QMimeData>
#include <QByteArray>
#include <QDataStream>
#include <QDrag>

#include "Settings.h"

PhotoModel::PhotoModel()
    : QStandardItemModel()
{
//    this->setFilter(QDir::NoDotAndDotDot | QDir::Files);
//    QStringList filters;
//    filters << "*.bmp";
//    this->setNameFilters(filters);
//    this->setNameFilterDisables(false);
}

void PhotoModel::setRootPath(const QString &directory)
{
    this->clear();
    m_directory.clear();

    m_directory = directory;

    QDir dir(directory);
    QFileInfoList fileList = dir.entryInfoList(QStringList() << "*.bmp", QDir::Files);
    int fileCount = fileList.size();
    for (int i=0; i<fileCount; ++i){
        QPixmap originalImage(fileList[i].filePath());
        if (!originalImage.isNull()){
            QPixmap scaledImage = originalImage.scaled(cher::APP_WIDGET_ICONSIZE_W * cher::DPI_SCALING,
                                                       cher::APP_WIDGET_ICONSIZE_H * cher::DPI_SCALING);
            this->setItem(i, new QStandardItem(QIcon(scaledImage), fileList[i].fileName()));
        }
    }
}

const QString &PhotoModel::getRootPath() const
{
    return m_directory;
}

Qt::DropActions PhotoModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

Qt::ItemFlags PhotoModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);
    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return defaultFlags;
}

QStringList PhotoModel::mimeTypes() const
{
    QStringList types;
    types << cher::MIME_PHOTO;
    return types;
}

QMimeData *PhotoModel::mimeData(const QModelIndexList &indexes) const
{
    if (m_directory.isEmpty())
        return 0;

    QMimeData* mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (const QModelIndex& index, indexes){
        if (index.isValid()){
            QString text = data(index, Qt::DisplayRole).toString();
            stream << text << m_directory;
        }
    }

    mimeData->setData(cher::MIME_PHOTO, encodedData);
    return mimeData;
}
