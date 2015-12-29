#ifndef __CanvasInfoModel_H__
#define __CanvasInfoModel_H__

#include <QAbstractTableModel>
#include <QString>
#include <QVector>

class CanvasInfo
{
public:
    CanvasInfo() {}
    void SetCanName(const QString &canName) { this->canName = canName; }
    QString GetCanName() const {return this->canName; }
private:
    QString canName;
    //add image list if needed
};

class CanvasInfoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    CanvasInfoModel(const int totalColumn, const int aColumnNumWithChechBox = 0, QObject *parent = 0)
        :totalColumn(totalColumn),colNumberWithCheckBox(aColumnNumWithChechBox),
        QAbstractTableModel(parent) {rowCheckStateMap.clear();};
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    void AddCanvasInfo(const CanvasInfo &canInfo);

signals:
    void StudentInfoIsChecked(const CanvasInfo &canInfo);
private:
    typedef QVector<CanvasInfo> CanvasInfos;
    CanvasInfos canInfos;
    int totalColumn;
    int colNumberWithCheckBox;
    QMap<int, Qt::CheckState> rowCheckStateMap;
};
#endif //__CanvasInfoModel_H__
