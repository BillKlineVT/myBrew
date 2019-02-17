#ifndef HOP_H
#define HOP_H

#include <QString>

class Hop
{
protected:

public:
    Hop();
    QString name;
    QString version;
    QString origin;
    double alpha;
    double amount;
    QString use;
    int time;
    QString notes;
    QString type;
    QString form;
    double beta;
    QString hsi;
    QString display_amount;
    QString inventory;
    QString display_time;

};

#endif // HOP_H
