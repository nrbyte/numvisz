#ifndef VISUALIZATIONSDAO_H
#define VISUALIZATIONSDAO_H

#include <QObject>

class VisualizationsDAO : public QObject
{
    Q_OBJECT
public:
    explicit VisualizationsDAO(QObject *parent = nullptr);

    QString getName(int index);
    int getRowCount();
};

#endif // VISUALIZATIONSDAO_H
