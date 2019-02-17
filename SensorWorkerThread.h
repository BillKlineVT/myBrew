#ifndef __SENSORWORKERTHREAD_H__
#define __SENSORWORKERTHREAD_H__
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
#include <vector>

using namespace std;

class SensorWorkerThread : public QThread
{
		Q_OBJECT
		
	public:
        SensorWorkerThread();
        void SensorWorker_Load_CSV_Data(QString csv_path, QString level_sensor_name);
		~SensorWorkerThread();
		void SensorWorker_Start();
	signals:
        void SensorWorker_READ_COMPLETE(double HLT_TEMP, double HLT_PWM, double Mash_TEMP, double Boil_TEMP, double Chill_TEMP, double Boil_Level, double HLT_Level, bool HLT_VALID, bool MASH_VALID, bool BOIL_VALID, bool CHILL_VALID, bool BOIL_LEVEL_VALID, bool HLT_LEVEL_VALID);

   protected:
      void run();
      int exec();
      double extrapolate_level_value(vector<double> calibration_temps, vector<double> calibration_volumes, vector<vector<int> > calibration_raw_data, double currentTemp, int current_raw_value);
   private:
	QMutex mutex;
	QWaitCondition condition;
	bool restart;
	bool abort;
	string HLTTempSensorLocation;
    string MashTempSensorLocation;
    string BoilTempSensorLocation;
    ifstream HLT_CURRENT_PID_DATA;
    ifstream Mash_TEMP_SENSOR_DATA;
    ifstream Boil_TEMP_SENSOR_DATA;
    ifstream Boil_LEVEL_SENSOR_DATA;
    ifstream HLT_LEVEL_SENSOR_DATA;
    QSqlDatabase *system_db;
    int previous_hlt_db_entry_id;
    int previous_mash_db_entry_id;
    int previous_chill_db_entry_id;
    int previous_boil_db_entry_id;
    int previous_boil_level_db_entry_id;
    int previous_hlt_level_db_entry_id;
    vector <double> boil_level_calibration_temps;
    vector <double> boil_level_calibration_volumes;
    vector < vector<int> > boil_level_calibration_raw_data;
    vector <double> hlt_level_calibration_temps;
    vector <double> hlt_level_calibration_volumes;
    vector < vector<int> > hlt_level_calibration_raw_data;
    bool boil_csv_data_loaded;
    bool hlt_csv_data_loaded;
    int time_of_last_boil_temp_entry;
    int time_of_last_hlt_temp_entry;
    int time_of_last_mash_temp_entry;
    int time_of_last_chill_temp_entry;
    int time_of_last_boil_level_entry;
    int time_of_last_hlt_level_entry;
};
#endif // __SENSORWORKERTHREAD_H__
