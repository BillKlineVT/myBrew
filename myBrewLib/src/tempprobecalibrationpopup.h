#ifndef TEMPPROBECALIBRATIONPOPUP_H
#define TEMPPROBECALIBRATIONPOPUP_H

#include <QDialog>

namespace Ui {
class TempProbeCalibrationPopup;
}

class TempProbeCalibrationPopup : public QDialog
{
    Q_OBJECT

public:
    explicit TempProbeCalibrationPopup(QWidget *parent = 0);
    ~TempProbeCalibrationPopup();
    double HLT_TempCorrection;
    double BK_TempCorrection;
    double HERMS_TempCorrection;
    double Chiller_TempCorrection;

public slots:
    void on_PB_TempCalibration_Save_clicked();

private slots:
    void on_PB_TempCalibration_Close_clicked();

private:
    Ui::TempProbeCalibrationPopup *ui;
};

#endif // TEMPPROBECALIBRATIONPOPUP_H
