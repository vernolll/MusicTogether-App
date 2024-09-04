#ifndef CUSTOMTABLEMODEL_H
#define CUSTOMTABLEMODEL_H

#include <QAbstractTableModel>
#include <QPair>

class CustomTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    CustomTableModel(const QList<QPair<QString, QString>>& values, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override; // Add this line to declare the new method

private:
    QList<QPair<QString, QString>> table_values;
};

#endif // CUSTOMTABLEMODEL_H
