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
}

void SideListModel::swapData(int item1, int item2){
    QString temp = m_list.at(item1);
    m_list[item1] = m_list.at(item2);
    m_list[item2] = temp;
    QModelIndex topLeft = createIndex(0,0);
    QModelIndex bottomRight = createIndex( m_list.count() ,0);
    emit dataChanged( topLeft, bottomRight );
}

Q_INVOKABLE int SideListModel::rowCount(const QModelIndex& parent/*=QModelindex()*/ ) const {
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
    if(m_list.length())
        insertRows(0, l.count());
}

void SideListModel::removeData(int item, int count){
    removeRows(item, count);
//    QModelIndex topLeft = createIndex(0,0);
//    QModelIndex bottomRight = createIndex( m_list.count() ,0);
//    emit dataChanged( topLeft, bottomRight );
}

void SideListModel::insertData(QString list){
    m_list.append(list);
    insertRows(m_list.count() - 1, 1);
}

Q_INVOKABLE QString SideListModel::getNameRegion (int i) {
    QModelIndex mIndex = index(i, 0, QModelIndex());

    return data(QModelIndex(mIndex), NameRole).toString() + "; " +
           data(QModelIndex(mIndex), RegionRole).toString();

}

Q_INVOKABLE QString SideListModel::getAge (int i) {
    QModelIndex mIndex = index(i, 0, QModelIndex());

    return data(QModelIndex(mIndex), AgeRole).toString();
}

Q_INVOKABLE QString SideListModel::getWeight (int i) {
    QModelIndex mIndex = index(i, 0, QModelIndex());

    return data(QModelIndex(mIndex), WeightRole).toString();
}

void SideListModel::clearModel(){
    int countRows = rowCount();
    if(countRows){
        removeRows(0, rowCount());
        m_list.clear();
    }
}

