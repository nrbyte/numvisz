#include "fontpickerdialog.h"
#include "ui_fontpickerdialog.h"

#include <QFileInfo>

#include <QPushButton>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftsnames.h>
#include <freetype/ttnameid.h>

#include <QStringConverter>
#include <QStandardPaths>
#include <QDirIterator>

FontPickerDialog::FontPickerDialog(std::shared_ptr<FontMap>& fonts,
                                   QWidget* parent)
    : QDialog(parent), ui(new Ui::FontPickerDialog), loadedFonts(fonts)
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

    QObject::connect(ui->listFamilies, &QListWidget::itemClicked, this,
                     &FontPickerDialog::fontFamilyClicked);
    QObject::connect(ui->listFiles, &QListWidget::itemClicked, this,
                     &FontPickerDialog::fontFileClicked);
}

FontPickerDialog::~FontPickerDialog() { delete ui; }

static QString loadSfntString(FT_UShort platformId, FT_UShort encodingId,
                              QByteArray characters)
{
    // Load the string from the SFNT table, changing the method based
    // on the encoding.
    //
    // NOTE: Only a few platforms/encoding combinations are supported
    //       here, an emptry string is returned if we were unable to
    //       decode it.
    QString str = "";
    if (platformId == 0)
    {
        // Apple Unicode
        auto toQt = QStringDecoder(QStringDecoder::Utf16BE);
        str = toQt(characters);
    }
    else if (platformId == 1)
    {
        // Macintosh
        str = QString(characters);
    }
    else if (platformId == 3)
    {
        // Microsoft
        auto toQt = QStringDecoder(QStringDecoder::Utf16BE);
        str = toQt(characters);
    }
    return str;
}

std::shared_ptr<FontMap> FontPickerDialog::loadFonts()
{
    // Initialise Freetype
    FT_Library library;
    FT_Init_FreeType(&library);

    auto loadedFonts = std::make_shared<FontMap>();

    // Go through every font in the system font directories
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
                // We have found a font file, load it into Freetype
                FT_Face face;
                FT_New_Face(library, filePath.toStdString().c_str(), 0, &face);

                // Load the Font Family entry of the SFNT table
                FT_SfntName sfnt;
                FT_Get_Sfnt_Name(face, TT_NAME_ID_FONT_FAMILY, &sfnt);
                QString familyName = loadSfntString(
                    sfnt.platform_id, sfnt.encoding_id,
                    QByteArray((const char*)sfnt.string, sfnt.string_len));

                // Load the Font type (e.g. Bold, Italic, etc.) from the SFNT
                // table
                FT_Get_Sfnt_Name(face, TT_NAME_ID_FONT_SUBFAMILY, &sfnt);
                QString fontType = loadSfntString(
                    sfnt.platform_id, sfnt.encoding_id,
                    QByteArray((const char*)sfnt.string, sfnt.string_len));

                // Unload the font
                FT_Done_Face(face);

                if (!(familyName.isEmpty() || fontType.isEmpty()))
                {
                    // Only add the font if we were able to successfully decode
                    // the family name and font type

                    // Add the font to the font mapping
                    (*loadedFonts)[familyName].push_back(
                        FontListing{filePath, fontType});
                }
            }
        }
    }

    FT_Done_FreeType(library);

    return loadedFonts;
}

void FontPickerDialog::fontFamilyClicked(QListWidgetItem* item)
{
    // Based on the font family clicked, display all font files from that family
    QList<FontListing> listings = ((*loadedFonts)[item->text()]);

    ui->listFiles->clear();
    for (auto& listing : listings)
    {
        ui->listFiles->addItem(listing.fontSubfamily);
    }

    // Disable the OK button, as it might have already been enabled from
    // clicking a previous font file
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
