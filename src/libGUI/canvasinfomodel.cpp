#include "canvasinfomodel.h"
#include <QDebug>
int CanvasInfoModel::rowCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    if (parent.isValid())
        return 0;
    return canInfos.size();
}

int CanvasInfoModel::columnCount( const QModelIndex &parent /*= QModelIndex()*/ ) const
{
    if (parent.isValid())
        return 0;
    return totalColumn;
}

QVariant CanvasInfoModel::data( const QModelIndex &index, int role ) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
            return canInfos[index.row()].GetCanName();
    }
    if (role == Qt::CheckStateRole)
    {
        if (index.column() == colNumberWithCheckBox)
        {
            if (rowCheckStateMap.contains(index.row()))
            return rowCheckStateMap[index.row()] == Qt::Checked ? Qt::Checked : Qt::Unchecked; return Qt::Unchecked;
        }
    }
    return QVariant();
}


Qt::ItemFlags CanvasInfoModel::flags( const QModelIndex &index ) const
{
    if
    (!index.isValid())
    return 0;


    if (index.column() == colNumberWithCheckBox)
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;


    return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


bool CanvasInfoModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if(!index.isValid())
        return false;
    if (role == Qt::CheckStateRole && index.column() == colNumberWithCheckBox)
    {
        if (value == Qt::Checked) //
        {
            rowCheckStateMap[index.row()] = Qt::Checked;
            qDebug()<<"the checked row is:"<<index.row();
            if(canInfos.size() > index.row())
            {
                emit StudentInfoIsChecked(canInfos[index.row()]);
            }

        }
        else
        {
            rowCheckStateMap[index.row()] = Qt::Unchecked;
            qDebug()<<"the unchecked row is:"<<index.row();
        }
    }
    return true;
}
QVariant CanvasInfoModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
            return QString("Canvas");
        if (section == 1)
            return QString("Visible");
    }
    return QVariant();
}

void CanvasInfoModel::AddCanvasInfo(const CanvasInfo &canInfo)
{
    canInfos.push_back(canInfo);  //add one piece of canvas info
    beginResetModel();  //initiate CanvasInfoModel update QTableView
    endResetModel();  //show what's inside canInfos
}
