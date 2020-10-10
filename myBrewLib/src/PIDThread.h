#ifndef __PIDTHREAD_H__
#define __PIDTHREAD_H__
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QMainWindow>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <QtSql>

using namespace std;

// place your code here
class PIDThread : public QThread
{
        Q_OBJECT

    public:
        PIDThread();
        ~PIDThread();
        void PID_Set(double PID_p_factor, double PID_i_factor,
            double PID_d_factor, double PID_setpoint);
        void PID_Stop();
    signals:
        //void PID_CYCLE_COMPLETE(double duty_cycle);
        //void SensorWorker_READ_COMPLETE(double HLT_TEMP, double Mash_TEMP, double Boil_TEMP);

   protected:
      void run();
      int exec();
   private:
    QMutex mutex;
    QWaitCondition condition;
    double p_factor;
    double i_factor;
    double d_factor;
    double setpoint;
    QSqlDatabase system_db;
};
#endif // __PIDTHREAD_H__
