#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>

#include "visualizationsdao.h"

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
    explicit AddDialog(std::shared_ptr<QMap<QString, QList<QString>>>& fonts,
                       QWidget* parent = nullptr);
    ~AddDialog();

private:
    Ui::AddDialog* ui;

    VisualizationsDao* viszDao;

    QString csvPath;
    QString fontPath;

    std::shared_ptr<QMap<QString, QList<QString>>> loadedFonts;

    // QDialog interface
public slots:
    void accept();

private slots:
    void chooseFile();
    void chooseFont();
};

#endif // ADDDIALOG_H
