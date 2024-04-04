#include "mainlistmodel.h"

#include <QDebug>

MainListModel::MainListModel(QObject* pobj)
    : QAbstractListModel(pobj) {

}

QHash<int, QByteArray> MainListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] =   "name";
    roles[RegionRole] = "region";
    roles[AgeRole] =    "age";
    roles[WeightRole] = "weight";
    return roles;
}

QVariant MainListModel::data(const QModelIndex& index, int nRole) const {
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

int MainListModel::rowCount(const QModelIndex& parent/*=QModelindex()*/ ) const {
    if (parent.isValid())
        return 0;
    return m_list.size();
}

Qt::ItemFlags MainListModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = QAbstractListModel::flags(index);
    return index.isValid() ? (flags | Qt::ItemIsEditable) : flags;
}

bool MainListModel::insertRows(int nRow, int nCount, const QModelIndex& parent/*=QModelindex()*/ ){

    if (parent.isValid())
        return false;
    beginInsertRows(QModelIndex(), nRow, nRow + nCount - 1);
    endInsertRows();
    return true;
}

bool MainListModel::removeRows(int nRow, int nCount, const QModelIndex& parent/*=QМodelindex()*/) {
    if (parent.isValid())
        return false;
    beginRemoveRows(QModelIndex(), nRow, nRow + nCount - 1);
        for (int i = 0; i < nCount; ++i)
            m_list.removeAt(nRow);
    endRemoveRows();
    return true;
}

void MainListModel::setList(QStringList l){
    m_list = l;
    if(insertRows(0, l.count()))
        qDebug() << "ok insert";
    else
        qDebug() << "no insert";

}

void MainListModel::moveItem(int item){
    removeRows(item, 1);
}

void MainListModel::clearModel(){
    removeRows(0, rowCount());
    m_list.clear();
}

void MainListModel::insertData(QString list){
    m_list.append(list);
    insertRows(m_list.count() - 1, 1);

}
