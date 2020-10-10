#ifndef BKPIDSETTINGSPOPUP_H
#define BKPIDSETTINGSPOPUP_H

#include <QDialog>

namespace Ui {
class BKPIDSettingsPopup;
}

class BKPIDSettingsPopup : public QDialog
{
    Q_OBJECT

public:
    explicit BKPIDSettingsPopup(QWidget *parent = 0);
    ~BKPIDSettingsPopup();
    double P_val;
    double I_val;
    double D_val;

public slots:
    void on_PB_BKPID_Save_clicked();
private slots:
    void on_PB_BKPID_Close_clicked();

private:
    Ui::BKPIDSettingsPopup *ui;
};

#endif // BKPIDSETTINGSPOPUP_H
