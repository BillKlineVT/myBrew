// adapted from Qt DigitalClock example
// widgets/digitalclock/digitalclock.h

#ifndef CLOCK_H
#define CLOCK_H

class Clock
{

    Q_OBJECT
public:
    Clock(QWidget *parent = 0);

private slots:
     void showTime(QWidget *parent = 0);
};

#endif // CLOCK_H
