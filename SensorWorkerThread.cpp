#include "SensorWorkerThread.h"
using namespace std;

SensorWorkerThread::SensorWorkerThread()
{
    this->time_of_last_boil_temp_entry = 0;
    this->time_of_last_hlt_temp_entry = 0;
    this->time_of_last_mash_temp_entry = 0;
    this->time_of_last_chill_temp_entry = 0;
    this->time_of_last_boil_level_entry = 0;
    this->time_of_last_hlt_level_entry = 0;

    this->system_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE","SensorThread"));
    this->system_db->setDatabaseName("/DB/brewing_db1.db");
    if(this->system_db->open())
        {
            qDebug() << "database connected to sensor thread!";
        }
        else
        {
            qDebug() << "sensor thread could not open database file:";
            qDebug() << this->system_db->lastError();
        }
}

void SensorWorkerThread::SensorWorker_Load_CSV_Data(QString csv_path, QString level_sensor_name)
{
    vector <double> level_calibration_volumes, level_calibration_temps;
    vector < vector <int> > level_calibration_raw_data;

    // read in csv data from table specified in config tab

    ifstream csv_file (csv_path.toUtf8()) ; // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
    vector<string> lines;
    int i = 0;
    while ( csv_file.good() )
    {
        lines.resize(i+1);
         getline ( csv_file, lines[i], '\n' ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
         //qDebug() << value; // display value removing the first and the last character from it
         i++;
    }
    if (lines.size() > 0)
    {
        // delete last line as it is empty
        lines.pop_back();
        // save line 0 to calibration_volumes vector
        std::stringstream ss(lines[0]);
        double value;
        while (ss >> value)
        {
            level_calibration_volumes.push_back(value);

            if (ss.peek() == ',')
                ss.ignore();
        }
        // rest of lines have cal data for a given temperature
        for (int i = 1;i<lines.size();i++)
        {
            level_calibration_raw_data.resize(i);
            std::stringstream ss(lines[i]);
            double value;
            // first entry in line is calibration temp
            int j=0;
            while (ss >> value)
            {
                if (j == 0)
                {
                    level_calibration_temps.push_back(value);
                }
                else
                {
                    level_calibration_raw_data[i-1].push_back(value);
                }
                j++;

                if (ss.peek() == ',')
                    ss.ignore();
            }
        }
    }

    if (level_sensor_name == "BOIL")
    {
        this->boil_level_calibration_volumes = level_calibration_volumes;
        this->boil_level_calibration_temps = level_calibration_temps;
        this->boil_level_calibration_raw_data = level_calibration_raw_data;
        if (lines.size() > 0)
            this->boil_csv_data_loaded = true;
        else
            this->boil_csv_data_loaded = false;
    }
    else if (level_sensor_name == "HLT")
    {
        this->hlt_level_calibration_volumes = level_calibration_volumes;
        this->hlt_level_calibration_temps = level_calibration_temps;
        this->hlt_level_calibration_raw_data = level_calibration_raw_data;
        if (lines.size() > 0)
            this->hlt_csv_data_loaded = true;
        else
            this->hlt_csv_data_loaded = false;
    }
    else
    {
        return;
    }

}

SensorWorkerThread::~SensorWorkerThread()
{
    //this->system_db.close();
}

void SensorWorkerThread::run()
{
	exec();
}

double SensorWorkerThread::extrapolate_level_value(vector<double> calibration_temps, vector<double> calibration_volumes, vector< vector<int> > calibration_raw_data, double currentTemp, int current_raw_value)
{
    int left_bound_temp_index=0;
    int right_bound_temp_index=0;
    bool found = false;

    // calibration_raw_data two-dim array, first dim is temperature of cal data, second dim is raw values of cal data

    // search thru calibration temps to see where current temp falls
    for (int i=0; i< calibration_temps.size() && !found;i++)
    {
        if(currentTemp <= calibration_temps[i])
        {
            // cur temp between temp[i] and temp[i-1]
            left_bound_temp_index = i-1;
            right_bound_temp_index = i;
            found = true;
        }
    }
    // case where temp is below or equal to lowest calibration data
    if (right_bound_temp_index == 0)
    {
        for(int i=0; i < calibration_raw_data[0].size();i++)
        {
            if (current_raw_value <= calibration_raw_data[0][i])
            {
                // if current raw is less than first cal data entry, return 0 as it is probably empty
                if (i==0)
                {
                    return 0.0;
                }
                // cur raw value between calibration_raw_data[0][i-1] and calibration_raw_data[0][i]
                double volumeScaleFactor = ((double)current_raw_value - (double)calibration_raw_data[0][i-1]) / ((double)calibration_raw_data[0][i] - (double)calibration_raw_data[0][i-1]);
                double current_volume_gals = (1-volumeScaleFactor) * calibration_volumes[i-1] + volumeScaleFactor * calibration_volumes[i];
                return current_volume_gals;
            }
            else if (i == (calibration_raw_data[0].size()-1) )
            {
                // if higher than the highest entry in cal data, assume it's full
                return calibration_volumes[calibration_volumes.size()-1];
            }
        }
    }
    for(int i=0; i < calibration_raw_data[left_bound_temp_index].size(); i++)
    {
        // extrapolate data based on distance from upper and lower bounds
        double tempScaleFactor = (currentTemp - calibration_temps[left_bound_temp_index]) / (calibration_temps[right_bound_temp_index] - calibration_temps[left_bound_temp_index]);
        double cur_index_extrapolated_raw_upper_bound = (1-tempScaleFactor) * (double)calibration_raw_data[left_bound_temp_index][i] + tempScaleFactor * (double)calibration_raw_data[right_bound_temp_index][i];
        double cur_index_extrapolated_raw_lower_bound = (1-tempScaleFactor) * (double)calibration_raw_data[left_bound_temp_index][i-1] + tempScaleFactor * (double)calibration_raw_data[right_bound_temp_index][i-1];
        if (current_raw_value <= cur_index_extrapolated_raw_upper_bound)
        {
            double volumeScaleFactor = ((double)current_raw_value - cur_index_extrapolated_raw_lower_bound) / (cur_index_extrapolated_raw_upper_bound - cur_index_extrapolated_raw_lower_bound);
            double current_volume_gals = (1-volumeScaleFactor)*calibration_volumes[i-1] + volumeScaleFactor * calibration_volumes[i];
            return current_volume_gals;
        }
        if ( i == (calibration_raw_data[left_bound_temp_index].size()-1) )
        {
            // if higher than the highest entry in cal data, assume it's full
            return calibration_volumes[calibration_volumes.size()-1];
        }
    }
    return 0;
}

int SensorWorkerThread::exec()
{
    double hlt_current_temp = 0.0;
    double hlt_current_PWM = 0.0;
    double mash_current_temp = 0.0;
    double boil_current_temp = 0.0;
    double chill_current_temp = 0.0;
    double boil_current_level = 0.0;
    double hlt_current_level = 0.0;

    bool HLT_VALID=false;
    bool MASH_VALID=false;
    bool BOIL_VALID=false;
    bool CHILL_VALID=false;
    bool BOIL_LEVEL_VALID=false;
    bool HLT_LEVEL_VALID=false;
    int db_entry_id = 0;
    while (1)
	{
        /*if(this->system_db->open())
        {
            qDebug() << "database connected to sensor thread!";
        }
        else
        {
            qDebug() << "sensor thread could not open database file:";
            qDebug() << this->system_db->lastError();
        }*/
        //qDebug() << "sensor worker thread loop";
        // query temp sensor databases to get most recent table entry for each sensor
        if(this->system_db->isOpen())
        {
            // do stuff if successfully opened
            QSqlQuery query(*this->system_db);
            query.exec("SELECT Id, HLT_current_temp, HLT_current_PWM from hlt_current_pid_data WHERE Id=(SELECT MAX(Id) FROM hlt_current_pid_data)");
            while(query.next())
            {
                int current_time = QDateTime::currentDateTime().toTime_t();
                db_entry_id = query.value(0).toInt();
                if (db_entry_id > this->previous_hlt_db_entry_id)
                {
                    this->time_of_last_hlt_temp_entry = QDateTime::currentDateTime().toTime_t();
                    HLT_VALID = true;
                }
                else if ((current_time - this->time_of_last_hlt_temp_entry) > 5)
                {
                    HLT_VALID = false;
                    qDebug() << "hlt temp data is more than 5 secs old";
                    qDebug() << "current time: " << current_time<< " time of last entry: " << this->time_of_last_hlt_temp_entry;
                }
                this->previous_hlt_db_entry_id = db_entry_id;

                hlt_current_temp= query.value(1).toDouble();
                hlt_current_PWM = query.value(2).toDouble();
                if (hlt_current_temp == -1)
                {
                    HLT_VALID = false;
                }
                //hlt_current_PWM = -1;
                //qDebug() << "temp= " << hlt_current_temp;
                //qDebug() << "PWM= " << hlt_current_PWM;
            }
            query.exec("SELECT Id, mash_current_temp from mash_temp_sensor_data WHERE Id=(SELECT MAX(Id) FROM mash_temp_sensor_data)");
            while(query.next())
            {
                int current_time = QDateTime::currentDateTime().toTime_t();
                db_entry_id = query.value(0).toInt();
                if (db_entry_id > this->previous_mash_db_entry_id)
                {
                    this->time_of_last_mash_temp_entry = QDateTime::currentDateTime().toTime_t();
                    MASH_VALID = true;
                }
                else if ((current_time - this->time_of_last_boil_temp_entry) > 5)
                {
                    MASH_VALID = false;
                    qDebug() << "mash temp data is more than 5 secs old";
                    qDebug() << "current time: " << current_time<< " time of last entry: " << this->time_of_last_mash_temp_entry;
                }
                this->previous_mash_db_entry_id = db_entry_id;
                mash_current_temp= query.value(1).toDouble();
                if (mash_current_temp == -1)
                {
                    MASH_VALID = false;
                }
            }
            query.exec("SELECT Id, boil_current_temp from boil_temp_sensor_data WHERE Id=(SELECT MAX(Id) FROM boil_temp_sensor_data)");
            while(query.next())
            {
                int current_time = QDateTime::currentDateTime().toTime_t();
                db_entry_id = query.value(0).toInt();
                if (db_entry_id > this->previous_boil_db_entry_id)
                {
                    this->time_of_last_boil_temp_entry = QDateTime::currentDateTime().toTime_t();
                    BOIL_VALID = true;
                }
                else if ((current_time - this->time_of_last_boil_temp_entry) > 5)
                {
                    BOIL_VALID = false;
                    qDebug() << "boil temp data is more than 5 secs old";
                    qDebug() << "current time: " << current_time<< " time of last entry: " << this->time_of_last_boil_temp_entry;
                }
                this->previous_boil_db_entry_id = db_entry_id;
                boil_current_temp= query.value(1).toDouble();
                if (boil_current_temp == -1)
                {
                    BOIL_VALID = false;
                }
            }
            query.exec("SELECT Id, chill_current_temp from chill_temp_sensor_data WHERE Id=(SELECT MAX(Id) FROM chill_temp_sensor_data)");
            while(query.next())
            {
                int current_time = QDateTime::currentDateTime().toTime_t();
                db_entry_id = query.value(0).toInt();
                if (db_entry_id > this->previous_chill_db_entry_id)
                {
                    this->time_of_last_chill_temp_entry = QDateTime::currentDateTime().toTime_t();
                    CHILL_VALID = true;
                }
                else if ((current_time - this->time_of_last_chill_temp_entry) > 5)
                {
                    CHILL_VALID = false;
                    qDebug() << "chill temp data is more than 5 secs old";
                    qDebug() << "current time: " << current_time<< " time of last entry: " << this->time_of_last_chill_temp_entry;
                }
                this->previous_chill_db_entry_id = db_entry_id;
                chill_current_temp= query.value(1).toDouble();
                if (chill_current_temp == -1)
                {
                    CHILL_VALID = false;
                }
            }
            query.exec("SELECT Id, boil_current_level from boil_level_sensor_data WHERE Id=(SELECT MAX(Id) FROM boil_level_sensor_data)");
            while(query.next())
            {
                int current_time = QDateTime::currentDateTime().toTime_t();
                db_entry_id = query.value(0).toInt();
                if (db_entry_id > this->previous_boil_level_db_entry_id)
                {
                    this->time_of_last_boil_level_entry = QDateTime::currentDateTime().toTime_t();
                    BOIL_LEVEL_VALID = true;
                }
                else if ((current_time - this->time_of_last_boil_level_entry) > 5)
                {
                    BOIL_LEVEL_VALID = false;
                    qDebug() << "boil level data is more than 5 secs old";
                    qDebug() << "current time: " << current_time<< " time of last entry: " << this->time_of_last_boil_level_entry;
                }
                this->previous_boil_level_db_entry_id = db_entry_id;

                // calculate level based on raw sensor data using lookup table and current temp
                if (this->boil_csv_data_loaded && BOIL_LEVEL_VALID)
                {
                    int boil_current_level_raw= query.value(1).toDouble();
                    boil_current_level = extrapolate_level_value(this->boil_level_calibration_temps, this->boil_level_calibration_volumes, this->boil_level_calibration_raw_data, boil_current_temp, boil_current_level_raw);
                    //qDebug()<< "boil current level calculated: "<<boil_current_level;
                    // store calculated current level back to boil level db table
                    //QSqlQuery insert_query(*this->system_db);
                    query.prepare("INSERT INTO boil_level_sensor_data (calculated_volume)"
                                  "VALUES (:boil_current_level)");
                    query.bindValue(":boil_current_level", boil_current_level);

                    bool ret = query.exec();
                    if (ret)
                                {
                                // http://www.sqlite.org/c3ref/last_insert_rowid.html
                                int newID = query.lastInsertId().toInt();
                                //qDebug() << "insert success for ID:" << newID;
                                }
                    else
                    {
                        qDebug() << "error inserting calculated boil_current_level into db";
                        qDebug() << this->system_db->lastError();
                    }
                }
                else
                    qDebug()<< "boil current level csv not yet loaded";
            }
            query.exec("SELECT Id, hlt_current_level from hlt_level_sensor_data WHERE Id=(SELECT MAX(Id) FROM hlt_level_sensor_data)");
            while(query.next())
            {
                int current_time = QDateTime::currentDateTime().toTime_t();
                db_entry_id = query.value(0).toInt();
                if (db_entry_id > this->previous_hlt_level_db_entry_id)
                {
                    this->time_of_last_hlt_level_entry = QDateTime::currentDateTime().toTime_t();
                    HLT_LEVEL_VALID = true;
                }
                else if ((current_time - this->time_of_last_boil_level_entry) > 5)
                {
                    HLT_LEVEL_VALID = false;
                    qDebug() << "hlt level data is more than 5 secs old";
                    qDebug() << "current time: " << current_time<< " time of last entry: " << this->time_of_last_hlt_level_entry;
                }
                this->previous_hlt_level_db_entry_id = db_entry_id;

                // calculate level based on raw sensor data using lookup table and current temp
                if (this->hlt_csv_data_loaded && HLT_LEVEL_VALID)
                {
                    int hlt_current_level_raw= query.value(1).toDouble();
                    hlt_current_level = extrapolate_level_value(this->hlt_level_calibration_temps, this->hlt_level_calibration_volumes, this->hlt_level_calibration_raw_data, hlt_current_temp, hlt_current_level_raw);
                    // store calculated current level back to hlt level db table
                    //QSqlQuery insert_query(*this->system_db);
                    query.prepare("INSERT INTO hlt_level_sensor_data (calculated_volume)"
                                  "VALUES (:hlt_current_level)");
                    query.bindValue(":hlt_current_level", hlt_current_level);

                    bool ret = query.exec();
                    if (ret)
                                {
                                // http://www.sqlite.org/c3ref/last_insert_rowid.html
                                int newID = query.lastInsertId().toInt();
                                //qDebug() << "insert success for ID:" << newID;
                                }
                    else
                    {
                        qDebug() << "error inserting calculated hlt_current_level into db";
                        qDebug() << this->system_db->lastError();
                    }
                }
                else
                    qDebug()<< "hlt current level csv not yet loaded";
            }
        }
        //this->system_db->close();
        emit SensorWorker_READ_COMPLETE(hlt_current_temp, hlt_current_PWM, mash_current_temp, boil_current_temp, chill_current_temp, boil_current_level, hlt_current_level, HLT_VALID, MASH_VALID, BOIL_VALID, CHILL_VALID, BOIL_LEVEL_VALID, HLT_LEVEL_VALID);
        sleep(1);
	}
    return 0;
}
