// adapted from Qt DigitalClock example
// widgets/digitalclock/digitalclock.cpp

#include "clock.h"

Clock::Clock(QWidget *parent)
{
    QTimer *timer = new QTimer(this);
         connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
         timer->start(1000);

         showTime(this);
}

void Clock::showTime(QWidget *parent)
{
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm");
    if ((time.second() % 2) == 0)
        text[2] = ' ';
    //display(text);
}
