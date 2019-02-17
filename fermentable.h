#ifndef FERMENTABLE_H
#define FERMENTABLE_H

#include <QString>

class Fermentable
{
public:
    Fermentable();
    QString name;
    QString version;
    QString type;
    double amount;
    double yield;
    double color;
    bool add_after_boil;
    QString origin;
    QString supplier;
    QString notes;
    double coarse_fine_diff;
    double moisture;
    double diastatic_power;
    double protein;
    double max_in_batch;
    bool recommend_mash;
    double ibu_gal_per_lb;
    QString display_amount;
    QString inventory;
    double potential;
    QString display_color;
    QString extract_substitute;
};

#endif // FERMENTABLE_H
