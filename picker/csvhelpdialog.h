#ifndef CSVHELPDIALOG_H
#define CSVHELPDIALOG_H

#include <QDialog>

namespace Ui {
class CsvHelpDialog;
}

class CsvHelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CsvHelpDialog(QWidget *parent = nullptr);
    ~CsvHelpDialog();

private:
    Ui::CsvHelpDialog *ui;
};

#endif // CSVHELPDIALOG_H
