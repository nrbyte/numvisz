#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>

#include "visualizationsdao.h"
#include "fontpickerdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class AddDialog;
}
QT_END_NAMESPACE

class AddDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddDialog(std::shared_ptr<FontMap>& fonts,
                       QWidget* parent = nullptr);
    ~AddDialog();

private:
    Ui::AddDialog* ui;

    VisualizationsDao* viszDao;

    QString csvPath;
    QString fontPath;

    std::shared_ptr<FontMap> loadedFonts;

    // QDialog interface
public slots:
    void accept();

private slots:
    void chooseFile();
    void chooseFont();
};

#endif // ADDDIALOG_H
