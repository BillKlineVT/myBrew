#include "hltpidsettingspopup.h"
#include "ui_hltpidsettingspopup.h"

HLTPIDSettingsPopup::HLTPIDSettingsPopup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HLTPIDSettingsPopup)
{
    ui->setupUi(this);
}

HLTPIDSettingsPopup::~HLTPIDSettingsPopup()
{
    delete ui;
}

void HLTPIDSettingsPopup::on_PB_HLTPID_Save_clicked()
{
    this->P_val = ui->LE_Config_HLTPID_PVal->text().toDouble();
    this->I_val = ui->LE_Config_HLTPID_IVal->text().toDouble();
    this->D_val = ui->LE_Config_HLTPID_DVal->text().toDouble();
}

void HLTPIDSettingsPopup::on_PB_HLTPID_Close_clicked()
{
    this->close();
}
