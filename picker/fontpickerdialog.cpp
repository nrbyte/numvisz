#include "fontpickerdialog.h"
#include "ui_fontpickerdialog.h"

#include <QFileInfo>

#include <QPushButton>

#include <iostream>

FontPickerDialog::FontPickerDialog(std::shared_ptr<QMap<QString, QList<QString>>>& fonts, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FontPickerDialog)
    , loadedFonts(fonts)
{
    ui->setupUi(this);

    ui->listFamilies->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listFiles->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);

    // Add in all the font families
    for (auto it = fonts->begin(); it != fonts->end(); ++it)
    {
        ui->listFamilies->addItem(it.key());
    }

    QObject::connect(ui->listFamilies, &QListWidget::itemClicked, this, &FontPickerDialog::fontFamilyClicked);
    QObject::connect(ui->listFiles, &QListWidget::itemClicked, this, &FontPickerDialog::fontFileClicked);
}

FontPickerDialog::~FontPickerDialog()
{
    delete ui;
}

void FontPickerDialog::fontFamilyClicked(QListWidgetItem* item)
{
    // Based on the font family clicked, display all font files from that family
    QList<QString> files = ((*loadedFonts)[item->text()]);

    ui->listFiles->clear();
    for (auto& file : files)
    {
        QFileInfo info(file);
        ui->listFiles->addItem(info.baseName());
    }

    // Disable the OK button, as it might have already been enabled from clicking a previous font file
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
}

void FontPickerDialog::fontFileClicked(QListWidgetItem* item)
{
    // Find the font file's full path
    int row = item->listWidget()->currentRow();
    QString family = ui->listFamilies->currentItem()->text();
    // Set it to public variable
    selectedFontFile = (*loadedFonts)[family].at(row);

    // Enable the OK button
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(false);
}
