#include "visualizationsmodel.h"

#include <iostream>

VisualizationsModel::VisualizationsModel(QObject *parent)
    : QAbstractListModel{parent}
{
    viszDao = new VisualizationsDao(this);
}

int VisualizationsModel::rowCount(const QModelIndex &parent) const
{
    return viszDao->getRowCount();
}

QVariant VisualizationsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        return viszDao->getName(index.row());
    }

    return QVariant();
}
