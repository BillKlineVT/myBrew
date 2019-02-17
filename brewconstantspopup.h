#ifndef BREWCONSTANTSPOPUP_H
#define BREWCONSTANTSPOPUP_H

#include <QDialog>

namespace Ui {
class BrewConstantsPopup;
}

class BrewConstantsPopup : public QDialog
{
    Q_OBJECT

public:
    explicit BrewConstantsPopup(QWidget *parent = 0);
    int default_vorlauf_time;
    double temp_drop_from_hlt_to_herms_out;
    double temp_drop_from_herms_out_to_mlt_in;
    double HLT_heating_element_interlock_min_vol;
    double BK_heating_element_interlock_min_vol;
    double ground_water_temp;
    double boiling_temp;
    double mash_timer_step_threshold;
    double mash_thickness;
    double wort_shrinkage_factor;
    double boiloff_rate;
    double brew_kettle_trub_loss;
    double mash_tun_loss;
    double grain_absorption;
    double sparge_grain_absorption;
    double fermenter_loss;
    ~BrewConstantsPopup();

public slots:
    void on_PB_BrewConstants_Save_clicked();
private slots:
    void on_PB_BrewConstants_Close_clicked();

private:
    Ui::BrewConstantsPopup *ui;
};

#endif // BREWCONSTANTSPOPUP_H
