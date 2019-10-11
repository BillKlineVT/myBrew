#include "PIDThread.h"
using namespace std;



PIDThread::PIDThread()
{
    p_factor = 0.0;
    i_factor = 0.0;
    d_factor = 0.0;
    setpoint = 0.0;
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE","PIDThread");
    this->system_db = database;
    this->system_db.setDatabaseName("/DB/brewing_db1.db");
    if(this->system_db.open())
    {
        qDebug() << "database connected to PIDThread!";
    }
    else
    {
        qDebug() << "Could not open database file:";
        qDebug() << this->system_db.lastError();
    }
}

PIDThread::~PIDThread()
{

}

void PIDThread::PID_Set(double PID_p_factor, double PID_i_factor,
    double PID_d_factor, double PID_setpoint)
{
    QMutexLocker locker(&mutex);

    this->p_factor = PID_p_factor;
    this->i_factor = PID_i_factor;
    this->d_factor = PID_d_factor;
    this->setpoint = PID_setpoint;

//    if (system_db.isOpen())
//    {
//        QSqlQuery query;
//        query.exec(QString("insert into hlt_pid_settings values(NULL, %1, %2, %3, %4)")
//                         .arg(PID_setpoint).arg(PID_p_factor).arg(PID_i_factor).arg(PID_d_factor));
//    }
}

void PIDThread::run()
{
    exec();
}

int PIDThread::exec()
{

}

void PIDThread::PID_Stop()
{

}



