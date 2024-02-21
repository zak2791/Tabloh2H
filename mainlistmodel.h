#ifndef MAINLISTMODEL_H
#define MAINLISTMODEL_H

#include <QAbstractListModel>
//#include <QJsonObject>

class MainListModel : public QAbstractListModel {
    Q_OBJECT

private:
    QStringList m_list;
    //QList<QJsonValue>* list;

public:
    enum MainListRoles {
        NameRole = Qt::UserRole + 1,
        RegionRole,
        AgeRole,
        WeightRole
    };
    MainListModel(QObject* pobj = 0);

    virtual QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex& index, int nRole) const override;

//    bool setData(const QModelIndex& index,
//                 const QVariant&    value,
//                 int                nRole
//                 )  override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    //    QVariant headerData(int             nsection,
    //                        Qt::Orientation orientation,
    //                        int             nRole = Qt::DisplayRole
    //                        ) const;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool insertRows (int nRow,
                     int nCount,
                     const QModelIndex& parent = QModelIndex()
            )  override;

    bool removeRows(int nRow,
                    int nCount,
                    const QModelIndex& parent = QModelIndex()
            )  override;

    void setList(QStringList);

public slots:
    void moveItem(int);


};

#endif // MAINLISTMODEL_H
