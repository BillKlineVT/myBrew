#ifndef BREWSTEPDURATIONPOPUP_H
#define BREWSTEPDURATIONPOPUP_H

#include <QDialog>

namespace Ui {
class BrewStepDurationPopup;
}

class BrewStepDurationPopup : public QDialog
{
    Q_OBJECT

public:
    explicit BrewStepDurationPopup(QWidget *parent = 0);
    ~BrewStepDurationPopup();
    double duration_FillHLT;
    double rate_HeatStrikeWater;
    double duration_TransferStrikeWater;
    double duration_DoughIn;
    double duration_MashOut;
    double duration_BatchSparge;
    double duration_Vorlauf;
    double rate_HeatWortToBoil;
    double duration_Whirlpool;
    double duration_Chill;
    double duration_Clean;

public slots:
    void on_PB_BrewDuration_Save_clicked();

private slots:
    void on_PB_BrewDuration_Close_clicked();

private:
    Ui::BrewStepDurationPopup *ui;
};

#endif // BREWSTEPDURATIONPOPUP_H
