#include "bkpidsettingspopup.h"
#include "ui_bkpidsettingspopup.h"

BKPIDSettingsPopup::BKPIDSettingsPopup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BKPIDSettingsPopup)
{
    ui->setupUi(this);
}

BKPIDSettingsPopup::~BKPIDSettingsPopup()
{
    delete ui;
}

void BKPIDSettingsPopup::on_PB_BKPID_Save_clicked()
{
    this->P_val = ui->LE_Config_BKPID_PVal->text().toDouble();
    this->I_val = ui->LE_Config_BKPID_IVal->text().toDouble();
    this->D_val = ui->LE_Config_BKPID_DVal->text().toDouble();
}

void BKPIDSettingsPopup::on_PB_BKPID_Close_clicked()
{
    this->close();
}
