#include "tempprobecalibrationpopup.h"
#include "ui_tempprobecalibrationpopup.h"

TempProbeCalibrationPopup::TempProbeCalibrationPopup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TempProbeCalibrationPopup)
{
    ui->setupUi(this);
}

TempProbeCalibrationPopup::~TempProbeCalibrationPopup()
{
    delete ui;
}

void TempProbeCalibrationPopup::on_PB_TempCalibration_Save_clicked()
{
    this->HLT_TempCorrection = ui->Table_Config_TempProbeCalibration->item(0,0)->text().toDouble();
    this->HERMS_TempCorrection = ui->Table_Config_TempProbeCalibration->item(1,0)->text().toDouble();
    this->BK_TempCorrection = ui->Table_Config_TempProbeCalibration->item(2,0)->text().toDouble();
    this->Chiller_TempCorrection = ui->Table_Config_TempProbeCalibration->item(3,0)->text().toDouble();
}

void TempProbeCalibrationPopup::on_PB_TempCalibration_Close_clicked()
{
    this->close();
}
