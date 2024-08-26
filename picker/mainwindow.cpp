#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton>
#include <QModelIndex>
#include <QProcess>
#include <QStringList>
#include <QMessageBox>

#include <QFontDatabase>
#include <QDirIterator>

#include <QStandardPaths>
#include <QDesktopServices>

#include <iostream>

#include "visualizationsmodel.h"
#include "visualizationsdao.h"
#include "adddialog.h"
#include "fontpickerdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , viszDao(new VisualizationsDao(this))
{
    ui->setupUi(this);

    VisualizationsModel* model = new VisualizationsModel(this);
    ui->viszList->setModel(model);
    ui->viszList->setSelectionMode(QAbstractItemView::SingleSelection);
    QObject::connect(ui->viszList, &QAbstractItemView::clicked, this, &MainWindow::viszSelected);

    QObject::connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::addVisualization);
    QObject::connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::playVisualization);
    QObject::connect(ui->fontButton, &QPushButton::clicked, this, &MainWindow::changeFont);
    QObject::connect(ui->openCSVButton, &QPushButton::clicked, this, &MainWindow::openCSV);

    ui->playButton->setDisabled(true);
    ui->spinBarHeight->setDisabled(true);
    ui->spinTimePerCategory->setDisabled(true);
    ui->openCSVButton->setDisabled(true);

    ui->fontButton->setDisabled(true);
    ui->fontButton->setText("Change Font");

    // Load fonts available in system directories
    // Group font files by family
    loadedFonts = std::make_shared<QMap<QString, QList<QString>>>();
    QStringList fontLocations = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
    for (auto& path : fontLocations)
    {
        QDirIterator iter(path);
        while (iter.hasNext())
        {
            QString filePath = iter.next();
            if (filePath.endsWith(".ttf") || filePath.endsWith(".otf"))
            {
                // Get the family of the font
                int id = QFontDatabase::addApplicationFont(filePath);
                QStringList families = QFontDatabase::applicationFontFamilies(id);

                // Add the file to the respective families
                for (auto& family : families)
                {
                    (*loadedFonts)[family].push_back(filePath);
                }
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addVisualization()
{
    AddDialog* addDialog = new AddDialog(loadedFonts, this);
    if (addDialog->exec() == QDialog::Accepted)
    {
        // Tell the model the data is updated, it needs to refresh
        emit ui->viszList->model()->dataChanged(QModelIndex(), QModelIndex());
    }
}

void MainWindow::playVisualization()
{
    // Save the current parameters
    currentlySelected.barHeight = ui->spinBarHeight->value();
    currentlySelected.timePerCategory = ui->spinTimePerCategory->value();
    viszDao->updateEntry(currentlySelected);

    // Set the arguments to pass to the barchart process
    QStringList args;
    args << "-csv" << currentlySelected.csvPath << "-font" << currentlySelected.fontPath
         << "-barheight"
         << ui->spinBarHeight->cleanText()
         << "-timepercategory" << ui->spinTimePerCategory->cleanText();

    // Start the program
    QProcess* process = new QProcess(this);
    QObject::connect(process, &QProcess::readyReadStandardError, this, &MainWindow::playbackError);
    process->start("./barchart", args);
}

void MainWindow::changeFont()
{
    FontPickerDialog fontPicker(loadedFonts);
    if (fontPicker.exec() == QDialog::Accepted)
    {
        currentlySelected.fontPath = fontPicker.selectedFontFile;
        QFileInfo info(currentlySelected.fontPath);
        ui->fontButton->setText(info.baseName());
    }
}

void MainWindow::openCSV()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(currentlySelected.csvPath));
}

void MainWindow::viszSelected(const QModelIndex& index)
{
    int row = index.row();

    currentlySelected = viszDao->getEntry(row + 1);

    ui->spinBarHeight->setValue(currentlySelected.barHeight);
    ui->spinTimePerCategory->setValue(currentlySelected.timePerCategory);

    ui->playButton->setDisabled(false);
    ui->spinBarHeight->setDisabled(false);
    ui->spinTimePerCategory->setDisabled(false);
    ui->fontButton->setDisabled(false);
    ui->openCSVButton->setDisabled(false);

    QFileInfo info(currentlySelected.fontPath);
    ui->fontButton->setText(info.baseName());
}

void MainWindow::playbackError()
{
    // Multiple QProcesses could be connected to this slot, therefore
    // we need to get the sender
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    if (process == nullptr) return;

    QString msg = process->readAllStandardError();
    // Forward the standard error to the user
    QMessageBox msgBox;
    msgBox.setWindowTitle("Visualization");
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
}
