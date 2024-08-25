#include "adddialog.h"
#include "./ui_adddialog.h"

#include <iostream>

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>

AddDialog::AddDialog(QWidget* parent)
    : QDialog{parent}
    , ui(new Ui::AddDialog)
{
    ui->setupUi(this);

    ui->fileLabel->setText("");
    ui->fontLabel->setText("");
    ui->errorLabel->setText("");

    viszDao = new VisualizationsDao(this);

    QObject::connect(ui->chooseButton, &QPushButton::clicked,
                     this, &AddDialog::chooseFile);
    QObject::connect(ui->fontButton, &QPushButton::clicked,
                     this, &AddDialog::chooseFont);
}

void AddDialog::accept()
{
    QString name = ui->nameEdit->text();
    if (name.isEmpty() || csvPath.isEmpty() || fontPath.isEmpty()) {
        ui->errorLabel->setText("Please enter all fields");
        return;
    }

    // Add the entry using the DAO
    viszDao->addEntry(name, csvPath, fontPath);
    QDialog::accept();
}

void AddDialog::chooseFile()
{
    QString file = QFileDialog::getOpenFileName(this,
                                 "Open CSV",
                                 "",
                                 "CSV File (*.csv)");
    if (file.isEmpty()) return;

    QFileInfo fileInfo(file);
    ui->fileLabel->setText("You picked: " + fileInfo.fileName());
    csvPath = file;
}

void AddDialog::chooseFont()
{
    QString file = QFileDialog::getOpenFileName(this,
                                              "Pick a font file",
                                              QStandardPaths::writableLocation(QStandardPaths::FontsLocation),
                                                "Font file (*.ttf)",
                                                nullptr, QFileDialog::DontUseNativeDialog);

    QFileInfo fileInfo(file);
    ui->fontLabel->setText("You picked: " + fileInfo.fileName());
    fontPath = file;
}


