#ifndef HLTPIDSETTINGSPOPUP_H
#define HLTPIDSETTINGSPOPUP_H

#include <QDialog>

namespace Ui {
class HLTPIDSettingsPopup;
}

class HLTPIDSettingsPopup : public QDialog
{
    Q_OBJECT

public:
    explicit HLTPIDSettingsPopup(QWidget *parent = 0);
    ~HLTPIDSettingsPopup();
    double P_val;
    double I_val;
    double D_val;

public slots:
    void on_PB_HLTPID_Save_clicked();
private slots:
    void on_PB_HLTPID_Close_clicked();

private:
    Ui::HLTPIDSettingsPopup *ui;
};

#endif // HLTPIDSETTINGSPOPUP_H
