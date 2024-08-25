#ifndef VISUALIZATIONSDAO_H
#define VISUALIZATIONSDAO_H

#include <QObject>

struct VisualizationEntry
{
    QString name;
    QString csvPath;
    QString fontPath;
    int barHeight;
    int timePerCategory;
};

class VisualizationsDao : public QObject
{
    Q_OBJECT
public:
    explicit VisualizationsDao(QObject *parent = nullptr);

    QString getName(int index);
    int getRowCount();

    void addEntry(const QString& name, const QString& csvPath, const QString& fontPath);

    VisualizationEntry getEntry(int id);
};

#endif // VISUALIZATIONSDAO_H
