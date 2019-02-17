#include "brewstepdurationpopup.h"
#include "ui_brewstepdurationpopup.h"

BrewStepDurationPopup::BrewStepDurationPopup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrewStepDurationPopup)
{
    ui->setupUi(this);
}

BrewStepDurationPopup::~BrewStepDurationPopup()
{
    delete ui;
}

void BrewStepDurationPopup::on_PB_BrewDuration_Save_clicked()
{
    this->duration_FillHLT = ui->table_Config_StepTimes->item(0,0)->text().toDouble();
    this->rate_HeatStrikeWater = ui->table_Config_StepTimes->item(1,0)->text().toDouble();
    this->duration_TransferStrikeWater = ui->table_Config_StepTimes->item(2,0)->text().toDouble();
    this->duration_DoughIn = ui->table_Config_StepTimes->item(3,0)->text().toDouble();
    this->duration_MashOut = ui->table_Config_StepTimes->item(4,0)->text().toDouble();
    this->duration_BatchSparge = ui->table_Config_StepTimes->item(5,0)->text().toDouble();
    this->duration_Vorlauf = ui->table_Config_StepTimes->item(6,0)->text().toDouble();
    this->rate_HeatWortToBoil = ui->table_Config_StepTimes->item(7,0)->text().toDouble();
    this->duration_Whirlpool = ui->table_Config_StepTimes->item(8,0)->text().toDouble();
    this->duration_Chill = ui->table_Config_StepTimes->item(9,0)->text().toDouble();
    this->duration_Clean = ui->table_Config_StepTimes->item(10,0)->text().toDouble();
}

void BrewStepDurationPopup::on_PB_BrewDuration_Close_clicked()
{
    this->close();
}
