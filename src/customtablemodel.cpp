#include "../include/CustomTableModel.h"

CustomTableModel::CustomTableModel(const QList<QPair<QString, QString>>& values, QObject* parent)
    : QAbstractTableModel(parent), table_values(values) {}


int CustomTableModel::rowCount(const QModelIndex& parent) const
{
    return table_values.size();
}


int CustomTableModel::columnCount(const QModelIndex& parent) const
{
    return 2;
}


QVariant CustomTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= table_values.size())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
            return table_values.at(index.row()).first;
        else if (index.column() == 1)
            return table_values.at(index.row()).second;
    }

    return QVariant();
}


QVariant CustomTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return "Название";
            case 1:
                return "Код";
            default:
                break;
            }
        }
    }
    return QVariant();
}
