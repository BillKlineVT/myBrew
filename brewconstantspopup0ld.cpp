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
