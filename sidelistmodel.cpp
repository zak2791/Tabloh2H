#include "sidelistmodel.h"

#include <QDebug>

SideListModel::SideListModel(QObject* pobj)
    : QAbstractListModel(pobj) {

}

QHash<int, QByteArray> SideListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] =   "name";
    roles[RegionRole] = "reg";
    roles[AgeRole] =    "age";
    roles[WeightRole] = "weight";
    return roles;
}

QVariant SideListModel::data(const QModelIndex& index, int nRole) const {
    if (!index.isValid())
        return QVariant();
    if (index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    switch(nRole){
    case NameRole:
        return m_list.at(index.row()).split("\n")[0];
    case RegionRole:
        return m_list.at(index.row()).split("\n")[1];
    case AgeRole:
        return m_list.at(index.row()).split("\n")[2];
    case WeightRole:
        return m_list.at(index.row()).split("\n")[3];

    }

    //    return (nRole == Qt::DisplayRole || nRole == Qt::EditRole)
    //            ? m_list.at(index.row())
    //            :QVariant();
}

//bool SideListModel::setData(const QModelIndex& index,
//                            const QVariant& value,
//                            int   nRole) {

//    if (index.isValid() && nRole == Qt::EditRole) {
//        m_list.replace(index.row(), value.value<QString>());
//        emit dataChanged(index, index);
//        return true;
//    }
//    return false;
//}

int SideListModel::rowCount(const QModelIndex& parent/*=QModelindex()*/ ) const {
    if (parent.isValid())
        return 0;
    return m_list.size();
}

Qt::ItemFlags SideListModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = QAbstractListModel::flags(index);
    return index.isValid() ? (flags | Qt::ItemIsEditable) : flags;
}

bool SideListModel::insertRows(int nRow, int nCount, const QModelIndex& parent/*=QModelindex()*/ ){

    if (parent.isValid())
        return false;
    beginInsertRows(QModelIndex(), nRow, nRow + nCount - 1);
    //for (int i = 0; i < nCount; ++i)
    //    m_list.insert(nRow, QJsonObject());
    endInsertRows();
    return true;
}

bool SideListModel::removeRows(int nRow, int nCount, const QModelIndex& parent/*=QМodelindex()*/) {
    if (parent.isValid())
        return false;
    beginRemoveRows(QModelIndex(), nRow, nRow + nCount - 1);
        for (int i = 0; i < nCount; ++i)
            m_list.removeAt(nRow);
    endRemoveRows();
    return true;
}

void SideListModel::setList(QStringList l){
    m_list = l;
    if(insertRows(0, l.count()))
        qDebug() << "ok insert";
    else
        qDebug() << "no insert";

}

void SideListModel::removeData(int item){
    removeRows(item, 1);
}

void SideListModel::insertData(QString list){
    m_list.append(list);
    if(insertRows(m_list.count() - 1, 1))
        qDebug() << "ok insert2";
    else
        qDebug() << "no insert2";
    qDebug() << "m_list.count() "<<m_list.count();
}

Q_INVOKABLE QString SideListModel::getNameRegion (int i) {
    QModelIndex mIndex = index(i, 0, QModelIndex());

    return data(QModelIndex(mIndex), NameRole).toString() + "; " +
           data(QModelIndex(mIndex), RegionRole).toString();

}

