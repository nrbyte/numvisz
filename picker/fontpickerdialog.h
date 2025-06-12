#ifndef FONTPICKERDIALOG_H
#define FONTPICKERDIALOG_H

#include <QDialog>
#include <QListWidget>

namespace Ui
{
class FontPickerDialog;
}

struct FontListing
{
    QString filePath;
    QString fontSubfamily;
};

using FontMap = QMap<QString, QList<FontListing>>;

class FontPickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FontPickerDialog(std::shared_ptr<FontMap>& fonts,
                              QWidget* parent = nullptr);
    ~FontPickerDialog();

    FontListing selectedFontFile;

    static std::shared_ptr<FontMap> loadFonts();

private:
    Ui::FontPickerDialog* ui;
    std::shared_ptr<FontMap> loadedFonts;

private slots:
    void fontFamilyClicked(QListWidgetItem*);
    void fontFileClicked(QListWidgetItem*);
};

#endif // FONTPICKERDIALOG_H
