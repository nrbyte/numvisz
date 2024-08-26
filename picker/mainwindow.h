#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMap>
#include <QList>
#include <QStringList>

#include "visualizationsdao.h"

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    VisualizationsDao* viszDao;
    std::shared_ptr<QMap<QString, QList<QString>>> loadedFonts;

    VisualizationEntry currentlySelected;

private slots:
    void addVisualization();
    void playVisualization();
    void changeFont();

    void viszSelected(const QModelIndex&);
};
#endif // MAINWINDOW_H
