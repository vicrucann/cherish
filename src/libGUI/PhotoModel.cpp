#include "PhotoModel.h"

#include <QDir>
#include <QFileInfoList>
#include <QPixmap>

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
            QPixmap scaledImage = originalImage.scaled(cher::APP_WIDGET_ICONSIZE * cher::DPI_SCALING,
                                                       cher::APP_WIDGET_ICONSIZE * cher::DPI_SCALING);
            this->setItem(i, new QStandardItem(QIcon(scaledImage), fileList[i].baseName()));
        }
    }
}

const QString &PhotoModel::getRootPath() const
{
    return m_directory;
}
