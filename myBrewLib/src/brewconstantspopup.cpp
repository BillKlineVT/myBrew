#include "brewconstantspopup.h"
#include "ui_brewconstantspopup.h"

BrewConstantsPopup::BrewConstantsPopup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrewConstantsPopup)
{
    ui->setupUi(this);
}

BrewConstantsPopup::~BrewConstantsPopup()
{
    delete ui;
}

void BrewConstantsPopup::on_PB_BrewConstants_Save_clicked()
{
    this->default_vorlauf_time = ui->table_BrewConstants->item(0,0)->text().toInt();
    this->temp_drop_from_hlt_to_herms_out = ui->table_BrewConstants->item(1,0)->text().toDouble();
    this->temp_drop_from_herms_out_to_mlt_in = ui->table_BrewConstants->item(2,0)->text().toDouble();
    this->HLT_heating_element_interlock_min_vol = ui->table_BrewConstants->item(3,0)->text().toDouble();
    this->ground_water_temp = ui->table_BrewConstants->item(4,0)->text().toDouble();
    this->boiling_temp = ui->table_BrewConstants->item(5,0)->text().toDouble();
    this->mash_timer_step_threshold = ui->table_BrewConstants->item(6,0)->text().toDouble();
    this->mash_thickness = ui->table_BrewConstants->item(7,0)->text().toDouble();
    this->wort_shrinkage_factor = ui->table_BrewConstants->item(8,0)->text().toDouble();
    this->boiloff_rate = ui->table_BrewConstants->item(9,0)->text().toDouble();
    this->brew_kettle_trub_loss = ui->table_BrewConstants->item(10,0)->text().toDouble();
    this->mash_tun_loss = ui->table_BrewConstants->item(11,0)->text().toDouble();
    this->grain_absorption = ui->table_BrewConstants->item(12,0)->text().toDouble();
    this->sparge_grain_absorption = ui->table_BrewConstants->item(13,0)->text().toDouble();
    this->fermenter_loss = ui->table_BrewConstants->item(14,0)->text().toDouble();
    this->BK_heating_element_interlock_min_vol = ui->table_BrewConstants->item(15,0)->text().toDouble();
}

void BrewConstantsPopup::on_PB_BrewConstants_Close_clicked()
{
    this->close();
}
