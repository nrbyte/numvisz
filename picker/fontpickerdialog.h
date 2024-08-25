#ifndef FONTPICKERDIALOG_H
#define FONTPICKERDIALOG_H

#include <QDialog>
#include <QListWidget>

namespace Ui {
class FontPickerDialog;
}

class FontPickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FontPickerDialog(std::shared_ptr<QMap<QString, QList<QString>>>& fonts, QWidget *parent = nullptr);
    ~FontPickerDialog();

    QString selectedFontFile;

private:
    Ui::FontPickerDialog *ui;
    std::shared_ptr<QMap<QString, QList<QString>>> loadedFonts;

private slots:
    void fontFamilyClicked(QListWidgetItem*);
    void fontFileClicked(QListWidgetItem*);
};

#endif // FONTPICKERDIALOG_H
