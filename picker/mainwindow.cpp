#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton>
#include <QModelIndex>
#include <QProcess>
#include <QStringList>

#include <iostream>

#include "visualizationsmodel.h"
#include "visualizationsdao.h"
#include "adddialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    VisualizationsModel* model = new VisualizationsModel(this);
    ui->viszList->setModel(model);
    ui->viszList->setSelectionMode(QAbstractItemView::SingleSelection);

    QObject::connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::addVisualization);
    QObject::connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::playVisualization);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addVisualization()
{
    AddDialog* addDialog = new AddDialog(this);
    if (addDialog->exec() == QDialog::Accepted)
    {
        // Tell the model the data is updated, it needs to refresh
        emit ui->viszList->model()->dataChanged(QModelIndex(), QModelIndex());
    }
}

void MainWindow::playVisualization()
{
    // Fetch the selected visualization
    VisualizationsDao dao;
    auto selection = ui->viszList->selectionModel()->selectedRows();
    if (selection.isEmpty()) return;

    int row = selection.first().row()+1;
    VisualizationEntry entry = dao.getEntry(row);

    // Set the arguments to pass to the barchart process
    QStringList args;
    args << "-csv" << entry.csvPath << "-font" << entry.fontPath
         << "-barheight"
         << QString::number(entry.barHeight)
         <<"-timepercategory" << QString::number(entry.timePerCategory);

    // Start the program
    QProcess* process = new QProcess();
    process->start("./barchart", args);
}
