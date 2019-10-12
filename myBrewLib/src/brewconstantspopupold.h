#ifndef BREWCONSTANTSPOPUP_H
#define BREWCONSTANTSPOPUP_H

#include <QDialog>

namespace Ui {
class BrewConstantsPopup;
}

class BrewConstantsPopup : public QDialog
{
    Q_OBJECT

public:
    explicit BrewConstantsPopup(QWidget *parent = 0);
    ~BrewConstantsPopup();

private:
    Ui::BrewConstantsPopup *ui;
};

#endif // BREWCONSTANTSPOPUP_H
