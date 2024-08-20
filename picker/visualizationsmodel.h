#ifndef VISUALIZATIONSMODEL_H
#define VISUALIZATIONSMODEL_H

#include <QAbstractListModel>

#include "visualizationsdao.h"

class VisualizationsModel : public QAbstractListModel
{
public:
    explicit VisualizationsModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    VisualizationsDAO* viszDao;
};

#endif // VISUALIZATIONSMODEL_H
