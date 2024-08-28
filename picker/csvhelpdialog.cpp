#include "csvhelpdialog.h"
#include "ui_csvhelpdialog.h"

CsvHelpDialog::CsvHelpDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CsvHelpDialog)
{
    ui->setupUi(this);
}

CsvHelpDialog::~CsvHelpDialog()
{
    delete ui;
}
