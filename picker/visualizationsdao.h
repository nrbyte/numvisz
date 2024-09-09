#ifndef VISUALIZATIONSDAO_H
#define VISUALIZATIONSDAO_H

#include <QObject>

struct VisualizationEntry
{
    int id;
    QString name;
    QString csvPath;
    QString fontPath;
    int barHeight;
    int timePerCategory;
    int numOfDecimalPlaces;
    int lineThickness;
};

class VisualizationsDao : public QObject
{
    Q_OBJECT
public:
    explicit VisualizationsDao(QObject* parent = nullptr);

    void addEntry(const QString& name, const QString& csvPath,
                  const QString& fontPath);
    void updateEntry(VisualizationEntry& entry);
    void deleteEntry(VisualizationEntry& entry);

    VisualizationEntry getEntry(int id);
};

#endif // VISUALIZATIONSDAO_H
