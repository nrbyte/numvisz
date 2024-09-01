#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlQuery>

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

#include "visualizationsdao.h"
#include "adddialog.h"
#include "fontpickerdialog.h"
#include "csvhelpdialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      viszDao(new VisualizationsDao(this))
{
    ui->setupUi(this);

    QSqlQueryModel* model = new QSqlQueryModel(this);
    QSqlQuery query("SELECT id, name FROM Visualizations",
                    QSqlDatabase::database("visualizations"));
    model->setQuery(query);
    ui->viszList->setModel(model);
    ui->viszList->setModelColumn(1);
    ui->viszList->setSelectionMode(QAbstractItemView::SingleSelection);
    QObject::connect(ui->viszList, &QAbstractItemView::clicked, this,
                     &MainWindow::viszSelected);

    QObject::connect(ui->addButton, &QPushButton::clicked, this,
                     &MainWindow::addVisualization);
    QObject::connect(ui->playButton, &QPushButton::clicked, this,
                     &MainWindow::playVisualization);
    QObject::connect(ui->fontButton, &QPushButton::clicked, this,
                     &MainWindow::changeFont);
    QObject::connect(ui->openCSVButton, &QPushButton::clicked, this,
                     &MainWindow::openCSV);
    QObject::connect(ui->deleteButton, &QPushButton::clicked, this,
                     &MainWindow::deleteEntry);
    disableButtons();

    QObject::connect(ui->actionExit, &QAction::triggered, this,
                     &MainWindow::menuExit);
    QObject::connect(ui->actionAbout_Qt, &QAction::triggered, this,
                     &MainWindow::helpQt);
    QObject::connect(ui->actionCSV_Format, &QAction::triggered, this,
                     &MainWindow::helpCsv);

    // Load fonts available in system directories
    // Group font files by family
    loadedFonts = std::make_shared<QMap<QString, QList<QString>>>();
    QStringList fontLocations =
        QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
    for (auto& path : fontLocations)
    {
        QDirIterator iter(path, QDirIterator::Subdirectories);
        while (iter.hasNext())
        {
            QString filePath = iter.next();
            if (filePath.endsWith(".ttf") || filePath.endsWith(".otf"))
            {
                // Get the family of the font
                int id = QFontDatabase::addApplicationFont(filePath);
                QStringList families =
                    QFontDatabase::applicationFontFamilies(id);

                // Add the file to the respective families
                for (auto& family : families)
                {
                    (*loadedFonts)[family].push_back(filePath);
                }
            }
        }
    }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::disableButtons()
{
    ui->playButton->setDisabled(true);
    ui->spinBarHeight->setDisabled(true);
    ui->spinDecimalPlaces->setDisabled(true);
    ui->spinTimePerCategory->setDisabled(true);
    ui->openCSVButton->setDisabled(true);
    ui->fontButton->setDisabled(true);
    ui->deleteButton->setDisabled(true);

    ui->spinBarHeight->setValue(35);
    ui->spinTimePerCategory->setValue(1000);
    ui->fontButton->setText("Change Font");
}

void MainWindow::enableButtons()
{
    ui->playButton->setDisabled(false);
    ui->spinBarHeight->setDisabled(false);
    ui->spinTimePerCategory->setDisabled(false);
    ui->spinDecimalPlaces->setDisabled(false);
    ui->fontButton->setDisabled(false);
    ui->openCSVButton->setDisabled(false);
    ui->deleteButton->setDisabled(false);
}

void MainWindow::addVisualization()
{
    AddDialog* addDialog = new AddDialog(loadedFonts, this);
    if (addDialog->exec() == QDialog::Accepted)
    {
        // Let the model know of the new data by re-running the query
        QSqlQueryModel* model =
            qobject_cast<QSqlQueryModel*>(ui->viszList->model());
        model->setQuery(model->query().lastQuery(),
                        QSqlDatabase::database("visualizations"));
    }
}

void MainWindow::playVisualization()
{
    // Save the current parameters
    currentlySelected.barHeight = ui->spinBarHeight->value();
    currentlySelected.timePerCategory = ui->spinTimePerCategory->value();
    currentlySelected.numOfDecimalPlaces = ui->spinDecimalPlaces->value();
    viszDao->updateEntry(currentlySelected);

    // Set the arguments to pass to the barchart process
    QStringList args;
    args << "-csv" << currentlySelected.csvPath << "-font"
         << currentlySelected.fontPath << "-barheight"
         << ui->spinBarHeight->cleanText() << "-timepercategory"
         << ui->spinTimePerCategory->cleanText() << "-decimalplaces"
         << ui->spinDecimalPlaces->cleanText();

    // Start the program
    QProcess* process = new QProcess(this);
    QObject::connect(process, &QProcess::readyReadStandardError, this,
                     &MainWindow::playbackError);
    process->start(QCoreApplication::applicationDirPath() + "/barchart", args);
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

void MainWindow::deleteEntry()
{
    QMessageBox msgBox;
    msgBox.setText("Are you sure you want to delete this entry?");
    msgBox.setInformativeText("The CSV file itself will not be deleted");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    if (msgBox.exec() == QMessageBox::Yes)
    {
        // Delete the entry in the database
        viszDao->deleteEntry(currentlySelected);
        // Update the model
        QSqlQueryModel* model =
            qobject_cast<QSqlQueryModel*>(ui->viszList->model());
        model->setQuery(model->query().lastQuery(),
                        QSqlDatabase::database("visualizations"));
        // Disable the buttons
        disableButtons();
    }
}

void MainWindow::viszSelected(const QModelIndex& index)
{
    int row = qobject_cast<QSqlQueryModel*>(ui->viszList->model())
                  ->record(index.row())
                  .value(0)
                  .toInt();
    currentlySelected = viszDao->getEntry(row);

    ui->spinBarHeight->setValue(currentlySelected.barHeight);
    ui->spinTimePerCategory->setValue(currentlySelected.timePerCategory);
    ui->spinDecimalPlaces->setValue(currentlySelected.numOfDecimalPlaces);

    enableButtons();

    QFileInfo info(currentlySelected.fontPath);
    ui->fontButton->setText(info.baseName());
}

void MainWindow::menuExit() { QApplication::quit(); }

void MainWindow::helpQt() { QApplication::aboutQt(); }

void MainWindow::helpCsv()
{
    CsvHelpDialog dialog;
    dialog.exec();
}

void MainWindow::playbackError()
{
    // Multiple QProcesses could be connected to this slot, therefore
    // we need to get the sender
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    if (process == nullptr)
        return;

    QString msg = process->readAllStandardError();
    // Forward the standard error to the user
    QMessageBox msgBox;
    msgBox.setWindowTitle("Visualization");
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
}
