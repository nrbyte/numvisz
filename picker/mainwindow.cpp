#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQueryModel>
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>
#include <QSqlQuery>

#include "visualizationsmodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    VisualizationsModel* model = new VisualizationsModel(this);

    ui->viszList->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}
