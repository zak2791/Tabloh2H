#ifndef SIDELISTMODEL_H
#define SIDELISTMODEL_H

#include <QAbstractListModel>

class SideListModel : public QAbstractListModel {
    Q_OBJECT

private:
    QStringList m_list;

public:
    enum MainListRoles {
        NameRole = Qt::UserRole + 1,
        RegionRole,
        AgeRole,
        WeightRole
    };
    SideListModel(QObject* pobj = 0);

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

    //virtual bool setData(const QModelIndex&, const QVariant&, int) override;


    void removeData(int, int count = 1);
    void insertData(QString);
    Q_INVOKABLE QString getNameRegion(int);
    //Q_INVOKABLE int get(int);
public slots:
    void swapData(int, int);


};

#endif // SIDELISTMODEL_H
