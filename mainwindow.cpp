#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // create all necessary timers
    this->clock_timer = new QTimer(this);
    this->elapsed_timer = new QTimer(this);
    this->boil_timer = new QTimer(this);
    this->postboil_timer = new QTimer(this);
    this->mash_timer = new QTimer(this);
    this->vorlauf_timer = new QTimer(this);
    this->cleancyclepump_timer = new QTimer(this);
    this->timer1_timer = new QTimer(this);
    this->timer2_timer = new QTimer(this);
    this->timer3_timer = new QTimer(this);
    this->timer4_timer = new QTimer(this);

    // start clock timer to update every second
    connect(this->clock_timer, SIGNAL(timeout()), this, SLOT(updateClockDisplay()));
    this->clock_timer->start(1000);

    // setup SensorWorkerThread to poll sensors
    connect(&SENSOR_THREAD, SIGNAL(SensorWorker_READ_COMPLETE(double, double, double, double, double, double, double, bool, bool, bool, bool, bool, bool)),
             this, SLOT(SensorHandler(double, double, double, double, double, double, double, bool, bool, bool, bool, bool, bool)));
    SENSOR_THREAD.start();

    // connect to system database
    this->system_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE","Main"));
    this->system_db->setDatabaseName("/DB/brewing_db1.db");
    if(this->system_db->open())
    {
        qDebug() << "database connected to main thread!";
    }
    else
    {
        qDebug() << "main thread could not open database file:";
        qDebug() << this->system_db->lastError();
        // TODO: check if database file exists here
    }

    // initialize GPIO vals
    this->GPIO_18_val = 0;
    this->GPIO_22_val = 0;
    this->GPIO_24_val = 0;
    this->GPIO_27_val = 0;

    // create customPlot_HLT graphs and assign data (0=HLT_Temp, 1=HLT_PWM, 2=HLT_SetTemp)
    ui->customPlot_HLT->addGraph();
    ui->customPlot_HLT->addGraph(ui->customPlot_HLT->xAxis, ui->customPlot_HLT->yAxis2);
    ui->customPlot_HLT->addGraph();
    ui->customPlot_HLT->yAxis2->setRange(0, 100);
    ui->customPlot_HLT->yAxis2->setVisible(true);
    ui->customPlot_HLT->graph(0)->setPen(QPen(Qt::red));
    ui->customPlot_HLT->graph(1)->setPen(QPen(Qt::blue));
    ui->customPlot_HLT->graph(2)->setPen(QPen(Qt::green));
    // give the axes some labels:
    ui->customPlot_HLT->xAxis->setLabel("time");
    ui->customPlot_HLT->yAxis->setLabel("temp F");
    // set default time range
    this->hlt_range_ticks = 60;
    ui->customPlot_HLT->xAxis->setTickStep(60);
    // set default value range
    ui->customPlot_HLT->yAxis->setRange(50, 185);

    // create customPlot_MLT graphs and assign data (0=HERMS Output, 1=HLT_Temp, 2=MASH_SetTemp)
    ui->customPlot_MLT->addGraph();
    ui->customPlot_MLT->addGraph();
    ui->customPlot_MLT->addGraph();
    ui->customPlot_MLT->graph(0)->setPen(QPen(Qt::red));
    ui->customPlot_MLT->graph(1)->setPen(QPen(Qt::blue));
    ui->customPlot_MLT->graph(2)->setPen(QPen(Qt::green));
    // give the axes some labels:
    ui->customPlot_MLT->xAxis->setLabel("time");
    ui->customPlot_MLT->yAxis->setLabel("temp F");
    // set default time range
    this->mlt_range_ticks = 60;
    ui->customPlot_MLT->xAxis->setTickStep(60);
    // set default value range
    ui->customPlot_MLT->yAxis->setRange(50, 185);

    // create customPlot_BK graphs and assign data (0=Boil Temp, 1=Boil PWM, 2=Boil_SetTemp)
    ui->customPlot_BK->addGraph();
    ui->customPlot_BK->addGraph(ui->customPlot_BK->xAxis, ui->customPlot_BK->yAxis2);
    ui->customPlot_BK->addGraph();
    ui->customPlot_BK->yAxis2->setRange(0, 100);
    ui->customPlot_BK->yAxis2->setVisible(true);
    ui->customPlot_BK->graph(0)->setPen(QPen(Qt::red));
    ui->customPlot_BK->graph(1)->setPen(QPen(Qt::blue));
    ui->customPlot_BK->graph(2)->setPen(QPen(Qt::green));
    // give the axes some labels:
    ui->customPlot_BK->xAxis->setLabel("time");
    ui->customPlot_BK->yAxis->setLabel("temp F");
    // set default time range
    this->bk_range_ticks = 60;
    ui->customPlot_BK->xAxis->setTickStep(60);
    // set default value range
    ui->customPlot_BK->yAxis->setRange(50, 220);

    // create customPlot_Chill graphs and assign data (0=Chill Temp, 1=Boil Temp, 2=PitchTemp)
    ui->customPlot_Chill->addGraph();
    ui->customPlot_Chill->addGraph();
    ui->customPlot_Chill->addGraph();
    ui->customPlot_Chill->graph(0)->setPen(QPen(Qt::red));
    ui->customPlot_Chill->graph(1)->setPen(QPen(Qt::blue));
    ui->customPlot_Chill->graph(2)->setPen(QPen(Qt::green));
    // give the axes some labels:
    ui->customPlot_Chill->xAxis->setLabel("time");
    ui->customPlot_Chill->yAxis->setLabel("temp F");
    // set default time range
    this->chill_range_ticks = 60;
    ui->customPlot_Chill->xAxis->setTickStep(60);
    // set default value range
    ui->customPlot_Chill->yAxis->setRange(50, 200);

    // create customPlot_Logs graphs and assign data (0=HLT_Temp, 1=HLT_PWM, 2=HERMS Output, 3=Boil Temp, 4=Boil PWM, 5=Chill Temp)
    ui->customPlot_Logs->addGraph();
    ui->customPlot_Logs->addGraph(ui->customPlot_Logs->xAxis, ui->customPlot_Logs->yAxis2);
    ui->customPlot_Logs->yAxis2->setRange(0, 100);
    ui->customPlot_Logs->yAxis2->setVisible(true);
    ui->customPlot_Logs->addGraph();
    ui->customPlot_Logs->addGraph();
    ui->customPlot_Logs->addGraph(ui->customPlot_Logs->xAxis, ui->customPlot_Logs->yAxis2);
    ui->customPlot_Logs->addGraph();
    ui->customPlot_Logs->graph(0)->setPen(QPen(Qt::red));
    ui->customPlot_Logs->graph(1)->setPen(QPen(Qt::blue));
    ui->customPlot_Logs->graph(2)->setPen(QPen(Qt::green));
    ui->customPlot_Logs->graph(3)->setPen(QPen(Qt::cyan));
    ui->customPlot_Logs->graph(4)->setPen(QPen(Qt::black));
    ui->customPlot_Logs->graph(5)->setPen(QPen(Qt::magenta));
    // give the axes some labels:
    ui->customPlot_Logs->xAxis->setLabel("time");
    ui->customPlot_Logs->yAxis->setLabel("temp F");
    // set default time range
    this->logs_range_ticks = 60;
    ui->customPlot_Logs->xAxis->setTickStep(60);
    // set default value range
    ui->customPlot_Logs->yAxis->setRange(50, 220);

    // intialize brew constants by directly calling update button to get default values
    this->BrewConstantsPopup_Window.on_PB_BrewConstants_Save_clicked();

    // initialize vorlauf timer based on config setting
    int minutes = this->BrewConstantsPopup_Window.default_vorlauf_time;
    QTime timer = QTime(0,minutes,0,0);
    ui->L_MLT_VorlaufTimer->setText(timer.toString());

    // initialize temp drop variable
    this->tempDropFromHLTtoHERMS = this->BrewConstantsPopup_Window.temp_drop_from_hlt_to_herms_out;

    // initialize current mash step
    this->current_mash_step = 0;
    this->MashStepTimer_StartTime = new QDateTime();
    this->MashStepTimer_EndTime = new QDateTime();

    // initialize logfile name
    this->CSV_fileName = "/home/pi/"+QDateTime::currentDateTime().date().toString("MMddyyyy")+"_BrewSession.csv";
    ui->LE_Logs_SaveLogAs->setText(this->CSV_fileName);

    // initialize alarm sound file
    this->Alarm_SoundFile = "/home/pi/alarm.wav";
    this->Audio_AlarmSettingsPopup_Window.alarm_sound_file_path = this->Alarm_SoundFile;

    // create checklist database
    this->createChecklist_DB();

    // initialize HLT PID settings
    this->HLT_PID_setpoint = ui->LE_HLT_PID_SetTemp->text().toDouble();
    this->HLT_PID_P_val = this->HLTPIDSettingsPopup_Window.P_val;
    this->HLT_PID_I_val = this->HLTPIDSettingsPopup_Window.I_val;
    this->HLT_PID_D_val = this->HLTPIDSettingsPopup_Window.D_val;
    this->HLT_PID_enable = ui->rb_HLT_PID_Enable->isChecked();
    ui->L_HLT_SetTemp->setText(QString::number(this->HLT_PID_setpoint));
    ui->L_HLT_SetTemp_Main->setText(QString::number(this->HLT_PID_setpoint));
    this->update_HLT_PID_Settings();

    // initialize MLT Settings
    ui->L_MLT_SetTemp->setText("0");
    ui->L_MLT_SetTemp_Main->setText("0");

    // initialize BK PID Settings (TODO)
    this->BK_PID_setpoint = 0.0;  // used temporarily when gas
    ui->LE_BK_SetTemp->setText(QString::number(this->BK_PID_setpoint));
    ui->L_BK_SetTemp->setText(QString::number(this->BK_PID_setpoint));
    ui->L_BK_SetTemp_Main->setText(QString::number(this->BK_PID_setpoint));
    this->update_BK_PID_Settings();

    // set fermentable table widths
    ui->table_Recipe_Fermentables->setColumnWidth(0,200);
    ui->table_Recipe_Fermentables->setColumnWidth(1,90);
    ui->table_Recipe_Fermentables->setColumnWidth(2,100);

    // set boil timer table widths
    ui->Table_Boil_HopTimer->setColumnWidth(0,75);
    ui->Table_Boil_HopTimer->setColumnWidth(1,75);
    ui->Table_Boil_HopTimer->setColumnWidth(2,75);
    ui->Table_Boil_HopTimer->setColumnWidth(3,50);

    // set postboil timer table widths
    ui->Table_PostBoil_HopTimer->setColumnWidth(0,75);
    ui->Table_PostBoil_HopTimer->setColumnWidth(1,75);
    ui->Table_PostBoil_HopTimer->setColumnWidth(2,75);
    ui->Table_PostBoil_HopTimer->setColumnWidth(3,50);

    //initialize total mash step rise time variable
    this->totalRiseTime = 0.0;

    //send level sensor calibration csv location to SensorWorker Thread
    this->boil_csv_path = "/home/pi/boil_level_cal_data.csv";
    ui->LE_Settings_Boil_Level_Cal_File->setText(this->boil_csv_path);
    this->hlt_csv_path = "/home/pi/hlt_level_cal_data.csv";
    ui->LE_Settings_HLT_Level_Cal_File->setText(this->hlt_csv_path);
    SENSOR_THREAD.SensorWorker_Load_CSV_Data(this->boil_csv_path, "BOIL");
    SENSOR_THREAD.SensorWorker_Load_CSV_Data(this->hlt_csv_path, "HLT");

    // initialize mash step temp threshold value
    this->mash_timer_step_start_threshold_temp = this->BrewConstantsPopup_Window.mash_timer_step_threshold;

    // hide mash hop CB unless recipe calls for it
    ui->CB_MLT_MashHopAddition->setEnabled(false);
    ui->CB_MLT_MashHopAddition->setVisible(false);

    // hide FW hop CB unless recipe calls for it
    ui->CB_MLT_FWHopAddition->setEnabled(false);
    ui->CB_MLT_FWHopAddition->setVisible(false);

    // initialize hlt&bk element protect interlock
    hlt_interlock_enabled = true;
    bk_interlock_enabled = true;

    // intialize brew step durations by directly calling update button to get default values
    this->BrewStepDurationPopup_Window.on_PB_BrewDuration_Save_clicked();

    // initialize temp probe calibrations
    this->TempProbeCalibrationPopup_Window.on_PB_TempCalibration_Save_clicked();

    // initialize HLT PID settings
    this->HLTPIDSettingsPopup_Window.on_PB_HLTPID_Save_clicked();

    // initialize BK PID settings
    this->BKPIDSettingsPopup_Window.on_PB_BKPID_Save_clicked();

    // initialize Sound and Alarm settings
    this->Audio_AlarmSettingsPopup_Window.on_PB_AudioAlarmSetting_Save_clicked();

    // initialize HLT autofill settings
    this->HLT_AutoFill_enabled = false;
    this->HLT_AutoFill_amount = 0.0;
    this->HLT_FillValve_shutoff_time_amount = 0.0;

    // initialize postboil timer
    this->max_postboil_time = 0;

    // initialize HLT autostart settings
    this->HLT_AutoStart_TimeToStartMode_enabled = true;
    this->HLT_AutoStart_TimeOfTempAchievedMode_enabled = false;
    this->HLT_AutoStart_enabled = false;
    this->HLT_AutoStart_StartTime = QDateTime();
}

MainWindow::~MainWindow()
{
    disconnect(&SENSOR_THREAD, SIGNAL(SensorWorker_READ_COMPLETE(double, double, double, double, double, double, double, bool, bool, bool, bool, bool, bool)),
             this, SLOT(SensorHandler(double, double, double, double, double, double, double, bool, bool, bool, bool, bool, bool)));
    delete &SENSOR_THREAD;
    QString db_ConnectionName = this->system_db->connectionName();
    this->system_db->close();
    delete this->system_db;
    QSqlDatabase::removeDatabase(db_ConnectionName);
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
   this->close();
}
void MainWindow::updateClockDisplay()
{
    //int time = QDateTime::currentDateTime().toLocalTime();
    ui->clock->setText(QDateTime::currentDateTime().toLocalTime().time().toString());
}
void MainWindow::updateElapsedDisplay()
{
    int elapsed_time = this->Checklist_StartTime->secsTo(QDateTime::currentDateTime());
    QTime deltaTime = QTime(0,0,0,0);
    deltaTime = deltaTime.addSecs(elapsed_time);
    ui->L_ElapsedTime_Display->setText(deltaTime.toString());
    //qDebug() << "elapsed time: " << elapsed_time;
    //qDebug() << "deltaTime: " << deltaTime.toString();
}

void MainWindow::updateBoilTimerDisplay()
{
    int time_remaining = QDateTime::currentDateTime().secsTo(*this->BoilTimer_EndTime);
    //qDebug() << "boiltimer endtime " << this->BoilTimer_EndTime->time().toString();
    if (time_remaining <= 0)
    {
        this->boil_timer->stop();
        ui->L_Boil_BoilTimer->setText("00:00:00");
        this->soundAlarm("Boil Complete");
    }
    else
    {
        QTime qtime_remaining = QTime(0,0,0,0);
        qtime_remaining = qtime_remaining.addSecs(time_remaining);
        ui->L_Boil_BoilTimer->setText(qtime_remaining.toString());
        if(ui->Table_Boil_HopTimer->rowCount() > 0)
        {
            int numRows = ui->Table_Boil_HopTimer->rowCount();
            for(int i=0;i<numRows;i++)
            {
                QTime qhop_time_remaining = QTime(0,0,0,0);
                QColor text_color;
                //qDebug() << "hop_timer_end " << hop_timer_end[i].time().toString();
                int hop_time_remaining = QDateTime::currentDateTime().secsTo(hop_timer_end[i]);
                if (hop_time_remaining > 0)
                {
                    qhop_time_remaining = qhop_time_remaining.addSecs(hop_time_remaining);
                    //qDebug() << "hop " << i <<" time remaining: " << hop_time_remaining;
                    //qDebug() << "qhop_time_remaining " << qhop_time_remaining.toString();
                    text_color = QColor::fromRgb(0,0,0);
                }
                else
                {
                    QFont font;
                    font.setBold(true);
                    text_color = QColor::fromRgb(255,0,0);
                    ui->Table_Boil_HopTimer->item(i,3)->setForeground(QColor::fromRgb(255,0,0));
                }
                ui->Table_Boil_HopTimer->setItem(i,3,new QTableWidgetItem(qhop_time_remaining.toString()));
                ui->Table_Boil_HopTimer->item(i,3)->setForeground(text_color);
            }
        }
    }
}

void MainWindow::updatePostBoilTimerDisplay()
{
    int time_remaining = QDateTime::currentDateTime().secsTo(*this->PostBoilTimer_EndTime);
    //qDebug() << "boiltimer endtime " << this->BoilTimer_EndTime->time().toString();
    if (time_remaining <= 0)
    {
        this->postboil_timer->stop();
        ui->L_Boil_PostBoilTimer->setText("00:00:00");
        this->soundAlarm("Post-Boil Complete");
    }
    else
    {
        QTime qtime_remaining = QTime(0,0,0,0);
        qtime_remaining = qtime_remaining.addSecs(time_remaining);
        ui->L_Boil_PostBoilTimer->setText(qtime_remaining.toString());
        if(ui->Table_PostBoil_HopTimer->rowCount() > 0)
        {
            //int numRows = ui->Table_Recipe_HopAdditions->rowCount();
            int numRows = ui->Table_PostBoil_HopTimer->rowCount();
            for(int i=0;i<numRows;i++)
            {
                QTime qhop_time_remaining = QTime(0,0,0,0);
                QColor text_color;
                //qDebug() << "hop_timer_end " << hop_timer_end[i].time().toString();
                int hop_time_remaining = QDateTime::currentDateTime().secsTo(hop_timer_end[i]);
                if (hop_time_remaining > 0)
                {
                    qhop_time_remaining = qhop_time_remaining.addSecs(hop_time_remaining);
                    //qDebug() << "hop " << i <<" time remaining: " << hop_time_remaining;
                    //qDebug() << "qhop_time_remaining " << qhop_time_remaining.toString();
                    text_color = QColor::fromRgb(0,0,0);
                }
                else
                {
                    QFont font;
                    font.setBold(true);
                    text_color = QColor::fromRgb(255,0,0);
                    ui->Table_PostBoil_HopTimer->item(i,3)->setForeground(QColor::fromRgb(255,0,0));
                }
                ui->Table_PostBoil_HopTimer->setItem(i,3,new QTableWidgetItem(qhop_time_remaining.toString()));
                ui->Table_PostBoil_HopTimer->item(i,3)->setForeground(text_color);
            }
        }
    }
}

void MainWindow::updateMashTimerDisplay()
{
    int numRows = ui->Table_Recipe_MashSteps->rowCount();
    QColor text_color;
    if(numRows > 0)
    {
        for(int i=0;i<numRows;i++)
        {
            if (i == this->current_mash_step)
            {
                // see if step timer has already started or if just getting here
                if(this->MashStepTimer_StartTime->isNull())
                {
                    // mash timer has not yet started
                    // see if still rising to step temp or if its time to start the timer countdown (take into account threshold temp window to avoid waiting for PID to reach exact step temp)
                    if(ui->L_MLT_CurrentTemp->text().toDouble() < (ui->Table_Mash_StepTimer->item(i,1)->text().toDouble() - this->BrewConstantsPopup_Window.mash_timer_step_threshold) )
                    {
                        // ensure set temp matches desired step temp
                        if(ui->L_MLT_SetTemp->text().toDouble() != ui->Table_Mash_StepTimer->item(i,1)->text().toDouble())
                        {
                            double step_temp = ui->Table_Recipe_MashSteps->item(i,1)->text().toDouble();
                            ui->L_MLT_SetTemp->setText(QString::number(step_temp));
                            ui->L_MLT_SetTemp_Main->setText(QString::number(step_temp));
                            ui->DSB_MLT_SetMashTemp->setValue(step_temp);
                            // set HLT PID to next step temp plus the drop between the HLT and the HERMS coil plus the temp delta between HERMS output and MLT
                            this->HLT_PID_setpoint = step_temp + this->BrewConstantsPopup_Window.temp_drop_from_hlt_to_herms_out + this->BrewConstantsPopup_Window.temp_drop_from_herms_out_to_mlt_in;
                            this->update_HLT_PID_Settings();
                            ui->L_HLT_SetTemp->setText(QString::number(this->HLT_PID_setpoint));
                            ui->L_HLT_SetTemp_Main->setText(QString::number(this->HLT_PID_setpoint));
                            ui->LE_HLT_PID_SetTemp->setText(QString::number(this->HLT_PID_setpoint));
                        }
                        // temp not yet at step temp
                        ui->L_MLT_MashStatus->setText("Waiting to achieve Step "+QString::number(this->current_mash_step + 1)+ " temperature");
                    }
                    else
                    {
                        // reached step temp, start countdown for this step
                        this->MashStepTimer_StartTime = new QDateTime(QDateTime::currentDateTime());
                        this->MashStepTimer_EndTime = new QDateTime(this->MashStepTimer_StartTime->addSecs(ui->Table_Recipe_MashSteps->item(i,0)->text().toInt()*60));
                    }
                }
                else
                {
                    // mash timer already running, update time left
                    int step_time_remaining = QDateTime::currentDateTime().secsTo(*this->MashStepTimer_EndTime);
                    QTime qstep_time_remaining = QTime(0,0,0,0);

                    if (step_time_remaining > 0)
                    {
                        // timer still has time left, update display
                        // set timer text to green to show it is current timer running
                        ui->L_MLT_MashStatus->setText("Step "+QString::number(this->current_mash_step+1)+ " underway");
                        text_color = QColor::fromRgb(0,255,0);
                        qstep_time_remaining = qstep_time_remaining.addSecs(step_time_remaining);

                        // also update overall mash timer if a timer is running
                        int overall_time_remaining = 0;
                        for (int j=this->current_mash_step+1;j<numRows;j++)
                        {
                            overall_time_remaining += ui->Table_Recipe_MashSteps->item(j,0)->text().toInt()*60;
                        }
                        QTime overall_qtime_remaining = QTime(0,0,0,0);
                        overall_qtime_remaining = overall_qtime_remaining.addSecs(overall_time_remaining + step_time_remaining);
                        ui->L_MLT_MashTimer->setText(overall_qtime_remaining.toString());
                    }
                    else
                    {
                        // timer has expired
                        // set timer text to red to signify timer has expired
                        text_color = QColor::fromRgb(255,0,0);
                        ui->Table_Mash_StepTimer->item(i,2)->setForeground(text_color);

                        // delete mashsteptimer objects so they are avail for next step
                        this->MashStepTimer_StartTime = new QDateTime();
                        this->MashStepTimer_EndTime = new QDateTime();

                        // advance to the next mash step
                        this->current_mash_step++;
                    }
                    ui->Table_Mash_StepTimer->setItem(i,2,new QTableWidgetItem(qstep_time_remaining.toString()));
                    ui->Table_Mash_StepTimer->item(i,2)->setForeground(text_color);
                }
            }
            else if (current_mash_step >= numRows)
            {
                // reached end of mash cycle
                ui->L_MLT_MashStatus->setText("Mash Complete");
                this->mash_timer->stop();
                ui->L_MLT_MashTimer->setText("00:00:00");
                this->soundAlarm("Mash Complete");
            }
        }
    }
}

void MainWindow::updateVorlaufTimerDisplay()
{
    int time_remaining = QDateTime::currentDateTime().secsTo(*this->VorlaufTimer_EndTime);
    if (time_remaining <= 0)
    {
        this->vorlauf_timer->stop();
        ui->L_MLT_VorlaufTimer->setText("00:00:00");
        this->soundAlarm("Vorlauf Complete");
        // if auto mode, stop pump
        if (this->GPIO_18_val != 0 && ui->RB_MLT_VorlaufTimer_Auto->isChecked())
        {
            qDebug() << "stopping pump!";
            this->GPIO_18_val = 0;
            update_GPIO_Settings();
        }

    }
    else
    {
        QTime qtime_remaining = QTime(0,0,0,0);
        qtime_remaining = qtime_remaining.addSecs(time_remaining);
        ui->L_MLT_VorlaufTimer->setText(qtime_remaining.toString());
        // if in auto mode, ensure pump is running
        if (this->GPIO_18_val != 1 && ui->RB_MLT_VorlaufTimer_Auto->isChecked())
        {
            qDebug() << "starting pump!";
            this->GPIO_18_val = 1;
            update_GPIO_Settings();
        }
    }
}

void MainWindow::updateCleanCyclePumpTimerDisplay()
{
    int time_remaining = QDateTime::currentDateTime().secsTo(*this->CleanCyclePumpTimer_EndTime);
    if (time_remaining <= 0)
    {
        this->cleancyclepump_timer->stop();
        ui->L_Clean_PumpTimer->setText("00:00:00");
        this->soundAlarm("Clean Cycle Complete");
        // if auto mode, stop pump
        if (this->GPIO_18_val != 0 && ui->RB_Clean_PumpTimer_Auto->isChecked())
        {
            qDebug() << "stopping pump!";
            this->GPIO_18_val = 0;
            update_GPIO_Settings();
        }

    }
    else
    {
        QTime qtime_remaining = QTime(0,0,0,0);
        qtime_remaining = qtime_remaining.addSecs(time_remaining);
        ui->L_Clean_PumpTimer->setText(qtime_remaining.toString());
        // if in auto mode, ensure pump is running
        if (this->GPIO_18_val != 1 && ui->RB_Clean_PumpTimer_Auto->isChecked())
        {
            qDebug() << "starting pump!";
            this->GPIO_18_val = 1;
            update_GPIO_Settings();
        }
    }
}

void MainWindow::updateTimer1Display()
{
    int time_remaining = QDateTime::currentDateTime().secsTo(*this->Timer1_EndTime);
    //qDebug() << "timer1 endtime " << this->Timer1_EndTime->time().toString();
    if (time_remaining <= 0)
    {
        this->timer1_timer->stop();
        ui->L_Timer1->setText("00:00:00");
        this->soundAlarm("Timer 1 Complete");
    }
    else
    {
        QTime qtime_remaining = QTime(0,0,0,0);
        qtime_remaining = qtime_remaining.addSecs(time_remaining);
        ui->L_Timer1->setText(qtime_remaining.toString());
    }
}

void MainWindow::updateTimer2Display()
{
    int time_remaining = QDateTime::currentDateTime().secsTo(*this->Timer2_EndTime);
    //qDebug() << "timer2 endtime " << this->Timer2_EndTime->time().toString();
    if (time_remaining <= 0)
    {
        this->timer2_timer->stop();
        ui->L_Timer2->setText("00:00:00");
        this->soundAlarm("Timer 2 Complete");
    }
    else
    {
        QTime qtime_remaining = QTime(0,0,0,0);
        qtime_remaining = qtime_remaining.addSecs(time_remaining);
        ui->L_Timer2->setText(qtime_remaining.toString());
    }
}

void MainWindow::updateTimer3Display()
{
    int time_remaining = QDateTime::currentDateTime().secsTo(*this->Timer3_EndTime);
    //qDebug() << "timer3 endtime " << this->Timer3_EndTime->time().toString();
    if (time_remaining <= 0)
    {
        this->timer3_timer->stop();
        ui->L_Timer3->setText("00:00:00");
        this->soundAlarm("Timer 3 Complete");
    }
    else
    {
        QTime qtime_remaining = QTime(0,0,0,0);
        qtime_remaining = qtime_remaining.addSecs(time_remaining);
        ui->L_Timer3->setText(qtime_remaining.toString());
    }
}

void MainWindow::updateTimer4Display()
{
    int time_remaining = QDateTime::currentDateTime().secsTo(*this->Timer4_EndTime);
    //qDebug() << "timer4 endtime " << this->Timer4_EndTime->time().toString();
    if (time_remaining <= 0)
    {
        this->timer4_timer->stop();
        ui->L_Timer4->setText("00:00:00");
        this->soundAlarm("Timer 4 Complete");
    }
    else
    {
        QTime qtime_remaining = QTime(0,0,0,0);
        qtime_remaining = qtime_remaining.addSecs(time_remaining);
        ui->L_Timer4->setText(qtime_remaining.toString());
    }
}

void MainWindow::on_PB_Checklist_Start_clicked()
{
    this->Checklist_StartTime = new QDateTime(QDateTime::currentDateTime());
    QDateTime current;
    //qDebug() << "starttime: "<<this->Checklist_StartTime->toLocalTime().time().toString();

    // Store Checklist StartTime into database
    this->insertChecklist_DB_Entry(this->Checklist_StartTime->toLocalTime().time().toString(), this->Checklist_StartTime->toTime_t(), "StartTime");

    ui->L_Checklist_StartTime->setText(this->Checklist_StartTime->toLocalTime().time().toString());
    ui->L_Checklist_StartTime->setEnabled(true);
    // start elapsed time counter/timer
    connect(this->elapsed_timer, SIGNAL(timeout()), this, SLOT(updateElapsedDisplay()));
    this->elapsed_timer->start(1000);
    ui->L_CurrentStep->setText("FILL HLT");
    this->calculate_ETC();
    ui->PB_Checklist_Start->setEnabled(false);

    // calculate projected finish of next step
    double minutes = this->BrewStepDurationPopup_Window.duration_FillHLT;
    QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
    ui->L_Checklist_FillHLT_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
    ui->L_Checklist_FillHLT_FinishTime->setEnabled(true);
}

void MainWindow::on_CB_Checklist_FillHLT_clicked()
{
    if(ui->CB_Checklist_FillHLT->isChecked())
    {
        ui->CB_Checklist_FillHLT->setEnabled(false);
        QDateTime FillHLT_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_FillHLT_Time->setText(FillHLT_time.toLocalTime().time().toString());
        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(FillHLT_time.toLocalTime().time().toString(), FillHLT_time.toTime_t(), "FillHLT");
        ui->L_Checklist_FillHLT_Time->setEnabled(true);
        ui->L_CurrentStep->setText("HEAT STRIKE");
        this->calculate_ETC();

        // calculate projected finish of next step
        double delta_temp = ui->LE_Recipe_StrikeTemp->text().toDouble() - this->BrewConstantsPopup_Window.ground_water_temp;
        //qDebug()<< "delta temp: "<<delta_temp;
        double minutes = delta_temp / this->BrewStepDurationPopup_Window.rate_HeatStrikeWater;
        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_HeatStrike_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_HeatStrike_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_HeatStrikeWater_clicked()
{
    if(ui->CB_Checklist_HeatStrikeWater->isChecked())
    {        
        ui->CB_Checklist_HeatStrikeWater->setEnabled(false);
        QDateTime HeatStrikeWater_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_HeatStrikeWater_Time->setText(HeatStrikeWater_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(HeatStrikeWater_time.toLocalTime().time().toString(), HeatStrikeWater_time.toTime_t(), "HeatStrikeWater");

        ui->L_Checklist_HeatStrikeWater_Time->setEnabled(true);
        ui->L_CurrentStep->setText("XFER STRIKE");
        this->calculate_ETC();

        // calculate projected finish of next step
        double minutes = this->BrewStepDurationPopup_Window.duration_TransferStrikeWater;
        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_TransferStrike_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_TransferStrike_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_TransferStrikeWater_clicked()
{
    if(ui->CB_Checklist_TransferStrikeWater->isChecked())
    {        
        ui->CB_Checklist_TransferStrikeWater->setEnabled(false);
        QDateTime TransferStrikeWater_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_TransferStrikeWater_Time->setText(TransferStrikeWater_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(TransferStrikeWater_time.toLocalTime().time().toString(), TransferStrikeWater_time.toTime_t(), "TransferStrikeWater");

        ui->L_Checklist_TransferStrikeWater_Time->setEnabled(true);
        ui->L_CurrentStep->setText("DOUGH IN");
        this->calculate_ETC();

        // calculate projected finish of next step
        double minutes = this->BrewStepDurationPopup_Window.duration_DoughIn;
        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_DoughIn_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_DoughIn_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_DoughIn_clicked()
{
    if(ui->CB_Checklist_DoughIn->isChecked())
    {
    ui->CB_Checklist_DoughIn->setEnabled(false);
        QDateTime DoughIn_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_DoughIn_Time->setText(DoughIn_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(DoughIn_time.toLocalTime().time().toString(), DoughIn_time.toTime_t(), "DoughIn");

        ui->L_Checklist_DoughIn_Time->setEnabled(true);
        ui->L_CurrentStep->setText("MASH");
        this->calculate_ETC();

        // calculate projected finish of next step

        // mash time is total mash timer time plus time to rise to each step
        double minutes = ui->L_Recipe_MashDuration->text().toDouble() + this->totalRiseTime/60;

        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_Mash_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_Mash_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_Mash_clicked()
{
    if(ui->CB_Checklist_Mash->isChecked())
    {
        ui->CB_Checklist_Mash->setEnabled(false);
        QDateTime Mash_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_Mash_Time->setText(Mash_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(Mash_time.toLocalTime().time().toString(), Mash_time.toTime_t(), "Mash");

        ui->L_Checklist_Mash_Time->setEnabled(true);
        ui->L_CurrentStep->setText("MASH OUT");
        this->calculate_ETC();

        // calculate projected finish of next step
        double minutes = this->BrewStepDurationPopup_Window.duration_MashOut;
        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_MashOut_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_MashOut_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_MashOut_clicked()
{
    if(ui->CB_Checklist_MashOut->isChecked())
    {
        ui->CB_Checklist_MashOut->setEnabled(false);
        QDateTime MashOut_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_MashOut_Time->setText(MashOut_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(MashOut_time.toLocalTime().time().toString(), MashOut_time.toTime_t(), "MashOut");

        ui->L_Checklist_MashOut_Time->setEnabled(true);
        ui->L_CurrentStep->setText("SPARGE 1");
        this->calculate_ETC();
        // calculate projected finish of next step
        double minutes = this->BrewStepDurationPopup_Window.duration_BatchSparge;
        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_BatchSparge1_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_BatchSparge1_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_BatchSparge1_clicked()
{
    if(ui->CB_Checklist_BatchSparge1->isChecked())
    {
        ui->CB_Checklist_BatchSparge1->setEnabled(false);
        QDateTime BatchSparge1_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_BatchSparge1_Time->setText(BatchSparge1_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(BatchSparge1_time.toLocalTime().time().toString(), BatchSparge1_time.toTime_t(), "BatchSparge1");

        ui->L_Checklist_BatchSparge1_Time->setEnabled(true);
        ui->L_CurrentStep->setText("VORLAUF 1");
        this->calculate_ETC();

        // calculate projected finish of next step
        double minutes = this->BrewStepDurationPopup_Window.duration_Vorlauf;
        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_Vorlauf1_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_Vorlauf1_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_Vorlauf1_clicked()
{
    if(ui->CB_Checklist_Vorlauf1->isChecked())
    {
        ui->CB_Checklist_Vorlauf1->setEnabled(false);
        QDateTime Vorlauf1_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_Vorlauf1_Time->setText(Vorlauf1_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(Vorlauf1_time.toLocalTime().time().toString(), Vorlauf1_time.toTime_t(), "Vorlauf1");

        ui->L_Checklist_Vorlauf1_Time->setEnabled(true);
        ui->L_CurrentStep->setText("SPARGE 2");
        this->calculate_ETC();

        // calculate projected finish of next step
        double minutes = this->BrewStepDurationPopup_Window.duration_BatchSparge;
        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_BatchSparge2_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_BatchSparge2_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_BatchSparge2_clicked()
{
    if(ui->CB_Checklist_BatchSparge2->isChecked())
    {
        ui->CB_Checklist_BatchSparge2->setEnabled(false);
        QDateTime BatchSparge2_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_BatchSparge2_Time->setText(BatchSparge2_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(BatchSparge2_time.toLocalTime().time().toString(), BatchSparge2_time.toTime_t(), "BatchSparge2");

        ui->L_Checklist_BatchSparge2_Time->setEnabled(true);
        ui->L_CurrentStep->setText("VORLAUF 2");
        this->calculate_ETC();

        // calculate projected finish of next step
        double minutes = this->BrewStepDurationPopup_Window.duration_Vorlauf;
        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_Vorlauf2_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_Vorlauf2_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_Vorlauf2_clicked()
{
    if(ui->CB_Checklist_Vorlauf2->isChecked())
    {
        ui->CB_Checklist_Vorlauf2->setEnabled(false);
        QDateTime Vorlauf2_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_Vorlauf2_Time->setText(Vorlauf2_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(Vorlauf2_time.toLocalTime().time().toString(), Vorlauf2_time.toTime_t(), "Vorlauf2");

        ui->L_Checklist_Vorlauf2_Time->setEnabled(true);
        ui->L_CurrentStep->setText("BOIL");
        this->calculate_ETC();

        // calculate projected finish of next step
        double delta_temp = this->BrewConstantsPopup_Window.boiling_temp - ui->L_BK_CurrentTemp->text().toDouble();
        //qDebug()<< "delta temp: "<<delta_temp;
        // time to end of boil is time to rise to boiling plus time of boil
        double minutes = delta_temp / this->BrewStepDurationPopup_Window.rate_HeatWortToBoil + ui->LE_Recipe_BoilTime->text().toDouble();
        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_Boil_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_Boil_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_Boil_clicked()
{
    if(ui->CB_Checklist_Boil->isChecked())
    {
        // Store Checklist entry into database

        ui->CB_Checklist_Boil->setEnabled(false);
        QDateTime Boil_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_Boil_Time->setText(Boil_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(Boil_time.toLocalTime().time().toString(), Boil_time.toTime_t(), "Boil");

        ui->L_Checklist_Boil_Time->setEnabled(true);
        ui->L_CurrentStep->setText("WHIRLPOOL");
        this->calculate_ETC();

        // calculate projected finish of next step
        double minutes = this->BrewStepDurationPopup_Window.duration_Whirlpool;
        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_Whirlpool_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_Whirlpool_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_Whirlpool_clicked()
{
    if(ui->CB_Checklist_Whirlpool->isChecked())
    {
        ui->CB_Checklist_Whirlpool->setEnabled(false);
        QDateTime Whirlpool_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_Whirlpool_Time->setText(Whirlpool_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(Whirlpool_time.toLocalTime().time().toString(), Whirlpool_time.toTime_t(), "Whirlpool");

        ui->L_Checklist_Whirlpool_Time->setEnabled(true);
        ui->L_CurrentStep->setText("CHILL");
        this->calculate_ETC();

        // calculate projected finish of next step
        double minutes = this->BrewStepDurationPopup_Window.duration_Chill;
        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_Chill_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_Chill_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_Chill_clicked()
{
    if(ui->CB_Checklist_Chill->isChecked())
    {
        // Store Checklist entry into database

        ui->CB_Checklist_Chill->setEnabled(false);
        QDateTime Chill_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_Chill_Time->setText(Chill_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(Chill_time.toLocalTime().time().toString(), Chill_time.toTime_t(), "Chill");

        ui->L_Checklist_Chill_Time->setEnabled(true);
        ui->L_CurrentStep->setText("CLEAN");
        this->calculate_ETC();

        // calculate projected finish of next step
        double minutes = this->BrewStepDurationPopup_Window.duration_Clean;
        QDateTime* Projected_Finish_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*minutes));
        ui->L_Checklist_Clean_FinishTime->setText(Projected_Finish_Time->toLocalTime().time().toString());
        ui->L_Checklist_Clean_FinishTime->setEnabled(true);
    }
}

void MainWindow::on_CB_Checklist_Clean_clicked()
{
    if(ui->CB_Checklist_Clean->isChecked())
    {
        // Store Checklist entry into database

        ui->CB_Checklist_Clean->setEnabled(false);
        QDateTime Clean_time = QDateTime(QDateTime::currentDateTime());
        ui->L_Checklist_Clean_Time->setText(Clean_time.toLocalTime().time().toString());

        // Store Checklist entry into database
        this->insertChecklist_DB_Entry(Clean_time.toLocalTime().time().toString(), Clean_time.toTime_t(), "Clean");

        ui->L_Checklist_Clean_Time->setEnabled(true);
        ui->L_CurrentStep->setText("COMPLETE");
        this->calculate_ETC();
    }
}

void MainWindow::calculate_ETC()
{
    double ETC_minutes = 0.0;
    if(!ui->CB_Checklist_FillHLT->isChecked())
    {
        ETC_minutes += this->BrewStepDurationPopup_Window.duration_FillHLT;
    }
    if(!ui->CB_Checklist_HeatStrikeWater->isChecked())
    {
        // calculate time to heat strike based on ground temp water and rate of heating in HLT
        double delta_temp = ui->LE_Recipe_StrikeTemp->text().toDouble() - this->BrewStepDurationPopup_Window.rate_HeatStrikeWater;
        //qDebug()<< "delta temp: "<<delta_temp;
        ETC_minutes += delta_temp / this->BrewStepDurationPopup_Window.rate_HeatStrikeWater;
    }
    if(!ui->CB_Checklist_TransferStrikeWater->isChecked())
    {
        ETC_minutes += this->BrewStepDurationPopup_Window.duration_TransferStrikeWater;
    }
    if(!ui->CB_Checklist_DoughIn->isChecked())
    {
        ETC_minutes += this->BrewStepDurationPopup_Window.duration_DoughIn;
    }
    if(!ui->CB_Checklist_Mash->isChecked())
    {
        ETC_minutes += ui->L_Recipe_MashDuration->text().toDouble() + this->totalRiseTime/60;
        // need to take into account rise time between steps
        /*for(int i=0;i<ui->Table_Recipe_MashSteps->rowCount();i++)
        {
            //ETC_minutes += ui->Table_Recipe_MashSteps->item(i,3)->text().toDouble();
        }*/
    }
    if(!ui->CB_Checklist_MashOut->isChecked())
    {
        ETC_minutes += this->BrewStepDurationPopup_Window.duration_MashOut;
    }
    if(!ui->CB_Checklist_BatchSparge1->isChecked())
    {
        ETC_minutes += this->BrewStepDurationPopup_Window.duration_BatchSparge;
    }
    if(!ui->CB_Checklist_Vorlauf1->isChecked())
    {
        ETC_minutes += this->BrewStepDurationPopup_Window.duration_Vorlauf;
    }
    if(!ui->CB_Checklist_BatchSparge2->isChecked())
    {
        ETC_minutes += this->BrewStepDurationPopup_Window.duration_BatchSparge;
    }
    if(!ui->CB_Checklist_Vorlauf2->isChecked())
    {
        ETC_minutes += this->BrewStepDurationPopup_Window.duration_Vorlauf;
    }
    if(!ui->CB_Checklist_Boil->isChecked())
    {
        // calculate time to heat to boil temp based on mash temp and heat rise rate for boil kettle
        double delta_temp = this->BrewStepDurationPopup_Window.rate_HeatWortToBoil - ui->L_Recipe_FirstMashTemp->text().toDouble();
        //qDebug()<< "delta temp: "<<delta_temp;
        //ETC_minutes += delta_temp / time->text().toDouble();
        ETC_minutes += delta_temp / this->BrewStepDurationPopup_Window.rate_HeatWortToBoil;
        ETC_minutes += ui->LE_Recipe_BoilTime->text().toDouble();
    }
    if(!ui->CB_Checklist_Whirlpool->isChecked())
    {
        ETC_minutes += this->BrewStepDurationPopup_Window.duration_Whirlpool;
    }
    if(!ui->CB_Checklist_Chill->isChecked())
    {
        ETC_minutes += this->BrewStepDurationPopup_Window.duration_Chill;
    }
    if(!ui->CB_Checklist_Clean->isChecked())
    {
        ETC_minutes += this->BrewStepDurationPopup_Window.duration_Clean;
    }
    //qDebug() << "ETC_minutes: " << ETC_minutes;

    ETC_Time = new QDateTime(QDateTime::currentDateTime().addSecs(60*ETC_minutes));
    ui->L_ETC_Time_Display->setText(ETC_Time->toLocalTime().time().toString());
}

void MainWindow::clear_and_disable_label(QLabel* label, QString new_label_text, bool enable_disable)
{
    label->setEnabled(enable_disable);
    label->setText(new_label_text);
}

void MainWindow::clear_and_disable_checkbox(QCheckBox* checkbox, bool checked, bool enable_disable)
{
    checkbox->setEnabled(enable_disable);
    checkbox->setChecked(checked);
}

void MainWindow::on_PB_Checklist_Reset_clicked()
{
    // reset all on this page to enabled and zero out times
    ui->PB_Checklist_Start->setEnabled(true);
    this->clear_and_disable_label(ui->L_Checklist_StartTime, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_FillHLT,false,true);
    this->clear_and_disable_label(ui->L_Checklist_FillHLT_FinishTime, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_FillHLT_Time, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_HeatStrikeWater,false,true);
    this->clear_and_disable_label(ui->L_Checklist_HeatStrike_FinishTime, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_HeatStrikeWater_Time, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_TransferStrikeWater,false,true);
    this->clear_and_disable_label(ui->L_Checklist_TransferStrike_FinishTime, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_TransferStrikeWater_Time, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_DoughIn_FinishTime, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_DoughIn,false,true);
    this->clear_and_disable_label(ui->L_Checklist_DoughIn_Time, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_Mash_FinishTime, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_Mash,false,true);
    this->clear_and_disable_label(ui->L_Checklist_Mash_Time, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_MashOut_FinishTime, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_MashOut,false,true);
    this->clear_and_disable_label(ui->L_Checklist_MashOut_Time, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_BatchSparge1_FinishTime, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_BatchSparge1,false,true);
    this->clear_and_disable_label(ui->L_Checklist_BatchSparge1_Time, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_Vorlauf1_FinishTime, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_Vorlauf1,false,true);
    this->clear_and_disable_label(ui->L_Checklist_Vorlauf1_Time, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_BatchSparge2_FinishTime, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_BatchSparge2,false,true);
    this->clear_and_disable_label(ui->L_Checklist_BatchSparge2_Time, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_Vorlauf2_FinishTime, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_Vorlauf2,false,true);
    this->clear_and_disable_label(ui->L_Checklist_Vorlauf2_Time, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_Boil_FinishTime, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_Boil,false,true);
    this->clear_and_disable_label(ui->L_Checklist_Boil_Time, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_Whirlpool_FinishTime, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_Whirlpool,false,true);
    this->clear_and_disable_label(ui->L_Checklist_Whirlpool_Time, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_Chill_FinishTime, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_Chill,false,true);
    this->clear_and_disable_label(ui->L_Checklist_Chill_Time, "<Time>", false);
    this->clear_and_disable_label(ui->L_Checklist_Clean_FinishTime, "<Time>", false);
    this->clear_and_disable_checkbox(ui->CB_Checklist_Clean,false,true);
    this->clear_and_disable_label(ui->L_Checklist_Clean_Time, "<Time>", false);
    this->elapsed_timer->stop();
    ui->L_ElapsedTime_Display->setText("<Time>");
    ui->L_ETC_Time_Display->setText("<Time>");
}

void MainWindow::on_LE_Recipe_BoilTime_editingFinished()
{
    //set boil timer based on recipe boil time entry
    ui->LE_Recipe_BoilTime->text().toInt();
    int hours = ui->LE_Recipe_BoilTime->text().toInt()/60;
    //ui->SB_Boil_BoilTimer_Hours->setValue(hours);
    int minutes = ui->LE_Recipe_BoilTime->text().toInt()-(hours*60);
    //ui->SB_Boil_BoilTimer_Mins->setValue(minutes);
    QTime timer = QTime(hours,minutes,0,0);
    ui->L_Boil_BoilTimer->setText(timer.toString());
}

void MainWindow::on_PB_Boil_BoilTimer_Start_clicked()
{
    this->BoilTimer_StartTime = new QDateTime(QDateTime::currentDateTime());
    this->BoilTimer_EndTime = new QDateTime(this->BoilTimer_StartTime->addSecs(ui->LE_Recipe_BoilTime->text().toInt()*60));
    int numRows = ui->Table_Boil_HopTimer->rowCount();
    // set timer start values if this is the first start
    for(int i=0;i<numRows;i++)
    {
        this->hop_timer_end.push_back(this->BoilTimer_StartTime->addSecs((ui->LE_Recipe_BoilTime->text().toInt() - ui->Table_Boil_HopTimer->item(i,3)->text().toInt())*60));
    }
    connect(this->boil_timer, SIGNAL(timeout()), this, SLOT(updateBoilTimerDisplay()));

    this->boil_timer->start(1000);
}

void MainWindow::on_PB_Boil_BoilTimer_Stop_clicked()
{
    this->boil_timer->stop();
    this->hop_timer_end.clear();
}

void MainWindow::on_PB_Recipe_AddHop_clicked()
{
    int numRows_config = ui->Table_Recipe_HopAdditions->rowCount();
    //ui->Table_Recipe_HopAdditions->setRowCount(numRows + 1);
    ui->Table_Recipe_HopAdditions->insertRow(numRows_config);
    ui->Table_Recipe_HopAdditions->setItem(numRows_config,0, new QTableWidgetItem(ui->LE_Recipe_AddHop_Time->text()));
    ui->Table_Recipe_HopAdditions->setItem(numRows_config,1, new QTableWidgetItem(ui->LE_Recipe_AddHop_Name->text()));
    ui->Table_Recipe_HopAdditions->setItem(numRows_config,2, new QTableWidgetItem(ui->LE_Recipe_AddHop_Type->text()));
    ui->Table_Recipe_HopAdditions->setItem(numRows_config,3, new QTableWidgetItem(ui->LE_Recipe_AddHop_Amount->text()));
    ui->Table_Recipe_HopAdditions->setItem(numRows_config,4, new QTableWidgetItem(ui->LE_Recipe_AddHop_Use->text()));

    // populate boil hoptimer if hops are of the "Boil" use-type
    if(ui->LE_Recipe_AddHop_Use->text() == "Boil")
    {
        int numRows_boil = ui->Table_Boil_HopTimer->rowCount();
        ui->Table_Boil_HopTimer->insertRow(numRows_boil);
        ui->Table_Boil_HopTimer->setItem(numRows_boil,0,new QTableWidgetItem(ui->LE_Recipe_AddHop_Amount->text()));
        ui->Table_Boil_HopTimer->setItem(numRows_boil,1,new QTableWidgetItem(ui->LE_Recipe_AddHop_Name->text()));
        ui->Table_Boil_HopTimer->setItem(numRows_boil,2,new QTableWidgetItem(ui->LE_Recipe_AddHop_Type->text()));
        ui->Table_Boil_HopTimer->setItem(numRows_boil,3,new QTableWidgetItem(ui->LE_Recipe_AddHop_Time->text()));
    }

    // handle aroma additions (post boil)
    if(ui->LE_Recipe_AddHop_Use->text() == "Aroma")
    {
        int numRows_postboil = ui->Table_PostBoil_HopTimer->rowCount();
        ui->Table_PostBoil_HopTimer->insertRow(numRows_postboil);
        ui->Table_PostBoil_HopTimer->setItem(numRows_postboil,0,new QTableWidgetItem(ui->LE_Recipe_AddHop_Amount->text()));
        ui->Table_PostBoil_HopTimer->setItem(numRows_postboil,1,new QTableWidgetItem(ui->LE_Recipe_AddHop_Name->text()));
        ui->Table_PostBoil_HopTimer->setItem(numRows_postboil,2,new QTableWidgetItem(ui->LE_Recipe_AddHop_Type->text()));
        ui->Table_PostBoil_HopTimer->setItem(numRows_postboil,3,new QTableWidgetItem(ui->LE_Recipe_AddHop_Time->text()));
        // find the longest postboil addition to set postboil timer
        if (ui->LE_Recipe_AddHop_Time->text().toInt() > this->max_postboil_time)
        {
            this->max_postboil_time = ui->LE_Recipe_AddHop_Time->text().toInt();
        }
    }


    // clear line edits
    ui->LE_Recipe_AddHop_Time->setText("");
    ui->LE_Recipe_AddHop_Name->setText("");
    ui->LE_Recipe_AddHop_Type->setText("");
    ui->LE_Recipe_AddHop_Amount->setText("");
    ui->LE_Recipe_AddHop_Use->setText("");

}

void MainWindow::on_PB_MLT_MashTimer_Start_clicked()
{
    connect(this->mash_timer, SIGNAL(timeout()), this, SLOT(updateMashTimerDisplay()));
    this->mash_timer->start(1000);
}

void MainWindow::on_PB_MLT_VorlaufTimer_Start_clicked()
{
    this->VorlaufTimer_StartTime = new QDateTime(QDateTime::currentDateTime());
    this->VorlaufTimer_EndTime = new QDateTime(this->VorlaufTimer_StartTime->addSecs(this->BrewConstantsPopup_Window.default_vorlauf_time*60));

    connect(this->vorlauf_timer, SIGNAL(timeout()), this, SLOT(updateVorlaufTimerDisplay()));
    this->vorlauf_timer->start(1000);


}

void MainWindow::on_PB_Recipe_AddStep_clicked()
{
    int numRows_config = ui->Table_Recipe_MashSteps->rowCount();

    ui->Table_Recipe_MashSteps->insertRow(numRows_config);
    ui->Table_Recipe_MashSteps->setItem(numRows_config,0, new QTableWidgetItem(ui->LE_Recipe_AddStep_Time->text()));
    ui->Table_Recipe_MashSteps->setItem(numRows_config,1, new QTableWidgetItem(ui->LE_Recipe_AddStep_Temperature->text()));
    ui->Table_Recipe_MashSteps->setItem(numRows_config,2, new QTableWidgetItem(ui->LE_Recipe_AddStep_Name->text()));
    double total_step_duration = 0.0;
    for(int i = 0;i<=numRows_config;i++)
    {
        total_step_duration += ui->Table_Recipe_MashSteps->item(i,0)->text().toDouble();
    }
    ui->L_Recipe_MashDuration->setText(QString::number(total_step_duration));
    int hours = ui->L_Recipe_MashDuration->text().toInt()/60;
    int minutes = ui->L_Recipe_MashDuration->text().toInt()-(hours*60);
    QTime timer = QTime(hours,minutes,0,0);
    ui->L_MLT_MashTimer->setText(timer.toString());

    double startTemp = 0.0;
    double endTemp = 0.0;
    // determine previous temperature before this step.  If it is the first step, rise time is negligible since strike temp will rise to first step very quickly.
    if (numRows_config == 0)
    {
        startTemp = ui->LE_Recipe_AddStep_Temperature->text().toDouble();
        ui->L_Recipe_FirstMashTemp->setText(ui->LE_Recipe_AddStep_Temperature->text());

        // set HLT to first mash temp and update PID with that set temp
        // take into account HLT to HERMS Out temp drop value as well
        double first_hlt_temp = ui->LE_Recipe_AddStep_Temperature->text().toDouble() + this->BrewConstantsPopup_Window.temp_drop_from_hlt_to_herms_out;
        ui->LE_HLT_PID_SetTemp->setText(QString::number(first_hlt_temp));
        ui->L_HLT_SetTemp->setText(QString::number(first_hlt_temp));
        ui->L_HLT_SetTemp_Main->setText(QString::number(first_hlt_temp));
        this->HLT_PID_setpoint = ui->LE_HLT_PID_SetTemp->text().toDouble();
        this->update_HLT_PID_Settings();
    }
    else
        startTemp = ui->Table_Recipe_MashSteps->item(numRows_config-1,1)->text().toDouble();
    endTemp = ui->LE_Recipe_AddStep_Temperature->text().toDouble();
    double deltaTemp = endTemp - startTemp;
    //qDebug() << "delta Temp "<<deltaTemp;
    // rise time is temp delta divided by HLT heating rate
    double riseTimeSeconds = (deltaTemp / this->BrewStepDurationPopup_Window.rate_HeatStrikeWater) * 60;
    // TODO: add this to a variable to use in checklist and ETC time calculations for total risetime
    this->totalRiseTime += riseTimeSeconds;
    //qDebug() << "riseTimeSeconds " << (int)riseTimeSeconds;
    QTime riseTime = QTime(0,0,0,0);
    riseTime = riseTime.addSecs((int)riseTimeSeconds);
    ui->Table_Recipe_MashSteps->setItem(numRows_config,3, new QTableWidgetItem(riseTime.toString()));
    //qDebug() << "riseTime " << riseTime.toString();

    int numRows_mash = ui->Table_Mash_StepTimer->rowCount();
    ui->Table_Mash_StepTimer->insertRow(numRows_mash);
    ui->Table_Mash_StepTimer->setItem(numRows_mash,0,new QTableWidgetItem(ui->LE_Recipe_AddStep_Name->text()));
    ui->Table_Mash_StepTimer->setItem(numRows_mash,1,new QTableWidgetItem(ui->LE_Recipe_AddStep_Temperature->text()));
    ui->Table_Mash_StepTimer->setItem(numRows_mash,2,new QTableWidgetItem(ui->LE_Recipe_AddStep_Time->text()));

    // clear line_edits
    ui->LE_Recipe_AddStep_Name->setText("");
    ui->LE_Recipe_AddStep_Temperature->setText("");
    ui->LE_Recipe_AddStep_Time->setText("");

}

void MainWindow::on_PB_MLT_MashTimer_Stop_clicked()
{
    this->mash_timer->stop();
}

void MainWindow::on_PB_MLT_VorlaufTimer_Stop_clicked()
{
    this->vorlauf_timer->stop();
    if (this->GPIO_18_val != 0 && ui->RB_MLT_VorlaufTimer_Auto->isChecked())
    {
        qDebug() << "stopping pump!";
        this->GPIO_18_val = 0;
        update_GPIO_Settings();
    }
}

void MainWindow::SensorHandler(double hlt_temp, double hlt_pwm, double mlt_temp, double boil_temp, double chill_temp, double boil_level, double hlt_level, bool HLT_VALID, bool MASH_VALID, bool BOIL_VALID, bool CHILL_VALID, bool BOIL_LEVEL_VALID, bool HLT_LEVEL_VALID)
{
    // update Calibration chart
    hlt_temp += this->TempProbeCalibrationPopup_Window.HLT_TempCorrection;
    mlt_temp += this->TempProbeCalibrationPopup_Window.HERMS_TempCorrection;
    boil_temp += this->TempProbeCalibrationPopup_Window.BK_TempCorrection;
    chill_temp += this->TempProbeCalibrationPopup_Window.Chiller_TempCorrection;

    // TODO - replace with real Boil PWM when implemented
    double boil_pwm = 0.0;

    // update displays of sensor values
    ui->L_HLT_CurrentTemp_Main->setText(QString::number(hlt_temp, 'f', 1));
    ui->L_HLT_CurrentTemp->setText(QString::number(hlt_temp, 'f', 1));
    ui->L_HLT_CurrentTemp_MLTPage->setText(QString::number(hlt_temp, 'f', 1));
    ui->L_HLT_CurrentTemp_BKPage->setText(QString::number(hlt_temp, 'f', 1));
    ui->L_MLT_CurrentTemp_Main->setText(QString::number(mlt_temp, 'f', 1));
    ui->L_MLT_CurrentTemp->setText(QString::number(mlt_temp, 'f', 1));
    ui->L_MLT_CurrentTemp_HLTPage->setText(QString::number(mlt_temp, 'f', 1));
    ui->L_MLT_CurrentTemp_BKPage->setText(QString::number(mlt_temp, 'f', 1));
    ui->L_BK_CurrentTemp_Main->setText(QString::number(boil_temp, 'f', 1));
    ui->L_BK_CurrentTemp->setText(QString::number(boil_temp, 'f', 1));
    ui->L_BK_CurrentTemp_HLTPage->setText(QString::number(boil_temp, 'f', 1));
    ui->L_BK_CurrentTemp_ChillPage->setText(QString::number(boil_temp, 'f', 1));
    ui->L_BK_CurrentTemp_MLTPage->setText(QString::number(boil_temp, 'f', 1));
    ui->L_Chill_Temp_Main->setText(QString::number(chill_temp, 'f', 1));
    ui->L_Chill_Temp->setText(QString::number(chill_temp, 'f', 1));
    ui->L_HLT_PWM->setText(QString::number(hlt_pwm, 'f', 1));

    // update BK level sensor values
    ui->L_BK_Volume->setText(QString::number(boil_level, 'f', 1) + " gal");
    ui->VS_BK_Volume->setSliderPosition(int(boil_level));
    ui->L_BK_Volume_Main->setText(QString::number(boil_level, 'f', 1) + " gal");
    ui->VS_BK_Volume_Main->setSliderPosition(int(boil_level));

    // update HLT level sensor values
    ui->L_HLT_Volume->setText(QString::number(hlt_level*4, 'f', 1) + " qt");
    ui->VS_HLT_Volume->setSliderPosition(int(hlt_level));
    ui->L_HLT_Volume_Main->setText(QString::number(hlt_level*4, 'f', 1) + " qt");
    ui->VS_HLT_Volume_Main->setSliderPosition(int(hlt_level));

    // check to see if data is stale (validity values)
    if (HLT_VALID)
    {
        ui->L_HLT_CurrentTemp_Main->setEnabled(true);
        ui->L_HLT_CurrentTemp->setEnabled(true);
    }
    else
    {
        ui->L_HLT_CurrentTemp_Main->setEnabled(false);
        ui->L_HLT_CurrentTemp->setEnabled(false);
    }

    if (MASH_VALID)
    {
        ui->L_MLT_CurrentTemp_Main->setEnabled(true);
        ui->L_MLT_CurrentTemp->setEnabled(true);
    }
    else
    {
        ui->L_MLT_CurrentTemp_Main->setEnabled(false);
        ui->L_MLT_CurrentTemp->setEnabled(false);
    }

    if (BOIL_VALID)
    {
        ui->L_BK_CurrentTemp_Main->setEnabled(true);
        ui->L_BK_CurrentTemp->setEnabled(true);
    }
    else
    {
        ui->L_BK_CurrentTemp_Main->setEnabled(false);
        ui->L_BK_CurrentTemp->setEnabled(false);
    }

    if (CHILL_VALID)
    {
        ui->L_Chill_Temp_Main->setEnabled(true);
        ui->L_Chill_Temp->setEnabled(true);
    }
    else
    {
        ui->L_Chill_Temp_Main->setEnabled(false);
        ui->L_Chill_Temp->setEnabled(false);
    }

    if (BOIL_LEVEL_VALID)
    {
        ui->L_BK_Volume->setEnabled(true);
        ui->L_BK_Volume_Main->setEnabled(true);
        ui->VS_BK_Volume->setEnabled(true);
        ui->VS_BK_Volume_Main->setEnabled(true);
    }
    else
    {
        ui->L_BK_Volume->setEnabled(false);
        ui->L_BK_Volume_Main->setEnabled(false);
        ui->VS_BK_Volume->setEnabled(false);
        ui->VS_BK_Volume_Main->setEnabled(false);
    }

    if (HLT_LEVEL_VALID)
    {
        ui->L_HLT_Volume->setEnabled(true);
        ui->L_HLT_Volume_Main->setEnabled(true);
        ui->VS_HLT_Volume->setEnabled(true);
        ui->VS_HLT_Volume_Main->setEnabled(true);
    }
    else
    {
        ui->L_HLT_Volume->setEnabled(false);
        ui->L_HLT_Volume_Main->setEnabled(false);
        ui->VS_HLT_Volume->setEnabled(false);
        ui->VS_HLT_Volume_Main->setEnabled(false);
    }

    // manage HLT AutoFill if enabled
    if(this->HLT_AutoFill_enabled)
    {
        if(HLT_LEVEL_VALID && hlt_level < (this->HLT_AutoFill_amount - this->HLT_FillValve_shutoff_time_amount))
        {
            // open valve if it isnt already
            if(this->GPIO_24_val != 1)
            {
                this->GPIO_24_val = 1;
                this->update_GPIO_Settings();
            }
        }
        else
        {
            // close valve if it isnt already
            if(this->GPIO_24_val != 0)
            {
                this->GPIO_24_val = 0;
                this->update_GPIO_Settings();
            }
        }

    }

    // add data to lines:
    double time = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    ui->customPlot_HLT->graph(0)->addData(time, hlt_temp);
    ui->customPlot_HLT->graph(1)->addData(time, hlt_pwm);
    ui->customPlot_HLT->graph(2)->addData(time, this->HLT_PID_setpoint);
    ui->customPlot_MLT->graph(0)->addData(time, mlt_temp);
    ui->customPlot_MLT->graph(1)->addData(time, hlt_temp);
    ui->customPlot_MLT->graph(2)->addData(time, ui->L_MLT_SetTemp->text().toDouble());
    ui->customPlot_BK->graph(0)->addData(time, boil_temp);
    ui->customPlot_BK->graph(1)->addData(time, boil_pwm);
    ui->customPlot_BK->graph(2)->addData(time, ui->L_BK_SetTemp->text().toDouble());
    ui->customPlot_Chill->graph(0)->addData(time, chill_temp);
    ui->customPlot_Chill->graph(1)->addData(time, boil_temp);
    ui->customPlot_Chill->graph(2)->addData(time, ui->LE_Recipe_PitchTemp->text().toDouble());
    ui->customPlot_Logs->graph(0)->addData(time, hlt_temp);
    ui->customPlot_Logs->graph(1)->addData(time, hlt_pwm);
    ui->customPlot_Logs->graph(2)->addData(time, mlt_temp);
    ui->customPlot_Logs->graph(3)->addData(time, boil_temp);
    ui->customPlot_Logs->graph(4)->addData(time, boil_pwm);
    ui->customPlot_Logs->graph(5)->addData(time, chill_temp);

    ui->customPlot_HLT->xAxis->setRange(time+0.25, this->hlt_range_ticks, Qt::AlignRight);
    ui->customPlot_HLT->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->customPlot_HLT->xAxis->setDateTimeFormat("hh:mm:ss");
    if(ui->HLT_Auto_Scale_Enable_ChkBox->isChecked())
    {
        ui->customPlot_HLT->graph(0)->rescaleValueAxis();
    }

    ui->customPlot_MLT->xAxis->setRange(time+0.25, this->mlt_range_ticks, Qt::AlignRight);
    ui->customPlot_MLT->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->customPlot_MLT->xAxis->setDateTimeFormat("hh:mm:ss");

    ui->customPlot_BK->xAxis->setRange(time+0.25, this->bk_range_ticks, Qt::AlignRight);
    ui->customPlot_BK->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->customPlot_BK->xAxis->setDateTimeFormat("hh:mm:ss");

    ui->customPlot_Chill->xAxis->setRange(time+0.25, this->chill_range_ticks, Qt::AlignRight);
    ui->customPlot_Chill->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->customPlot_Chill->xAxis->setDateTimeFormat("hh:mm:ss");

    ui->customPlot_Logs->xAxis->setRange(time+0.25, this->logs_range_ticks, Qt::AlignRight);
    ui->customPlot_Logs->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->customPlot_Logs->xAxis->setDateTimeFormat("hh:mm:ss");

    ui->customPlot_HLT->replot();
    ui->customPlot_MLT->replot();
    ui->customPlot_BK->replot();
    ui->customPlot_Chill->replot();
    ui->customPlot_Logs->replot();

    ui->L_BK_TimeUntilTempAchieved->setText(this->calculate_HeatTime(this->BK_PID_setpoint, boil_temp, this->BrewStepDurationPopup_Window.rate_HeatWortToBoil).toString());
    ui->L_HLT_TimeUntilTempAchieved->setText(this->calculate_HeatTime(this->HLT_PID_setpoint, hlt_temp, this->BrewStepDurationPopup_Window.rate_HeatStrikeWater).toString());

    this->boil_sensor_temp_history_time.push_back(time);
    this->boil_sensor_temp_history_value.push_back(boil_temp);
    if(this->boil_sensor_temp_history_value.size() ==10)
    {
        double cur_delta_temp_boil = this->boil_sensor_temp_history_value.back() - this->boil_sensor_temp_history_value.front();
        this->boil_sensor_temp_history_value.pop_front();
        double cur_delta_time_boil = this->boil_sensor_temp_history_time.back() - this->boil_sensor_temp_history_time.front();
        this->boil_sensor_temp_history_time.pop_front();
        // calculate rate of boil over last 10 seconds in deg/min
        double cur_boil_rate = cur_delta_temp_boil / (cur_delta_time_boil/60);
        double heating_rate_tolerance_factor_boil = 0.5;
        if(cur_boil_rate < (this->BrewStepDurationPopup_Window.rate_HeatWortToBoil - heating_rate_tolerance_factor_boil))
        {
            ui->L_BK_RateOfHeat_Main->setStyleSheet("QLabel { color : red; }");
            ui->L_BK_RateOfHeat->setStyleSheet("QLabel { color : red; }");
        }
        else
        {
            ui->L_BK_RateOfHeat_Main->setStyleSheet("QLabel { color : green; }");
            ui->L_BK_RateOfHeat->setStyleSheet("QLabel { color : green; }");
        }
        ui->L_BK_RateOfHeat_Main->setText(QString::number(cur_boil_rate, 'f', 1));
        ui->L_BK_RateOfHeat->setText(QString::number(cur_boil_rate, 'f', 1));
    }
    this->hlt_sensor_temp_history_time.push_back(time);
    this->hlt_sensor_temp_history_value.push_back(hlt_temp);
    if(this->hlt_sensor_temp_history_value.size() ==10)
    {
        double cur_delta_temp_hlt = this->hlt_sensor_temp_history_value.back() - this->hlt_sensor_temp_history_value.front();
        this->hlt_sensor_temp_history_value.pop_front();
        double cur_delta_time_hlt = this->hlt_sensor_temp_history_time.back() - this->hlt_sensor_temp_history_time.front();
        this->hlt_sensor_temp_history_time.pop_front();
        // calculate rate of hlt heating over last 10 seconds in deg/min
        double cur_hlt_rate = cur_delta_temp_hlt / (cur_delta_time_hlt/60);
        double heating_rate_tolerance_factor_hlt = 0.5;
        if(cur_hlt_rate < (this->BrewStepDurationPopup_Window.rate_HeatStrikeWater - heating_rate_tolerance_factor_hlt))
        {
            ui->L_HLT_RateOfHeat_Main->setStyleSheet("QLabel { color : red; }");
            ui->L_HLT_RateOfHeat->setStyleSheet("QLabel { color : red; }");
        }
        else
        {
            ui->L_HLT_RateOfHeat_Main->setStyleSheet("QLabel { color : green; }");
            ui->L_HLT_RateOfHeat->setStyleSheet("QLabel { color : green; }");
        }
        ui->L_HLT_RateOfHeat_Main->setText(QString::number(cur_hlt_rate, 'f', 1));
        ui->L_HLT_RateOfHeat->setText(QString::number(cur_hlt_rate, 'f', 1));
    }

    // HLT element protect interlock logic
    if(this->hlt_interlock_enabled)
        if(hlt_level * 4 < this->BrewConstantsPopup_Window.HLT_heating_element_interlock_min_vol)
        {
            // shut off PID for HLT if not already done
            if (!ui->rb_HLT_PID_Disable->isChecked())
            {
                ui->rb_HLT_PID_Disable->setChecked(true);
                this->on_rb_HLT_PID_Disable_clicked();
                // sound alarm
                this->soundAlarm("HLT Interlock");
                // change text of enable cb to red
            }
        }

    // BK element protect interlock logic
    if(this->bk_interlock_enabled)
        if(boil_level * 4 < this->BrewConstantsPopup_Window.BK_heating_element_interlock_min_vol)
        {
            // shut off PID for BK if not already done
            if (!ui->rb_BK_PID_Disable->isChecked())
            {
                ui->rb_BK_PID_Disable->setChecked(true);
                this->on_rb_BK_PID_Disable_clicked();
                // sound alarm
                this->soundAlarm("Boil Kettle Interlock");
                // change text of enable cb to red
            }
        }

}

QTime MainWindow::calculate_HeatTime(double setpoint, double current_temp, double heating_rate_per_min)
{
    double delta_temp = setpoint - current_temp;
    if(delta_temp >0)
    {
        double total_seconds_to_temp = delta_temp / heating_rate_per_min * 60;
        int hours_to_temp = total_seconds_to_temp/(60*60);
        int minutes_to_temp = total_seconds_to_temp/60 - hours_to_temp*60;
        int seconds_to_temp = total_seconds_to_temp - minutes_to_temp*60 - hours_to_temp*60*60;
        QTime t_time_to_temp = QTime(hours_to_temp,minutes_to_temp,seconds_to_temp);
        return t_time_to_temp;
    }
    else
    {
        return QTime(0,0,0);
    }
}

void MainWindow::HLT_PIDHandler(double setpoint, double P, double I, double D)
{

}

void MainWindow::BK_PIDHandler(double setpoint, double P, double I, double D)
{

}

void MainWindow::createChecklist_DB()
{
    //qDebug() << "creating checklist db";
}

void MainWindow::insertChecklist_DB_Entry(QString timestamp_local, int timestamp_epoch, QString Checklist_Entry)
{

    //qDebug() << "inserting checklist entry: " << Checklist_Entry <<" local: " << timestamp_local <<" epoch: "<< timestamp_epoch;

    QSqlQuery query(*this->system_db);
    query.prepare("INSERT INTO checklist_times ("+Checklist_Entry+"_timestamp_local, "+Checklist_Entry+"_timestamp_epoch)"
                  "VALUES (:timestamp_local, :timestamp_epoch)");
    query.bindValue(":timestamp_local", timestamp_local);
    query.bindValue(":timestamp_epoch", timestamp_epoch);

    bool ret = query.exec();
    if (ret)
                {
                // http://www.sqlite.org/c3ref/last_insert_rowid.html
                int newID = query.lastInsertId().toInt();
                //qDebug() << "insert success for ID:" << newID;
                }
    else
    {
        qDebug() << "error inserting checklist db entry into db";
        qDebug() << this->system_db->lastError();
    }
}

void MainWindow::update_GPIO_Settings()
{
   // if (this->system_db->isOpen())
  //  {
   //     qDebug() << "db is open, trying to insert...";
        QSqlQuery query(*this->system_db);
        query.prepare("INSERT INTO gpio_outputs (date, GPIO_18, GPIO_22, GPIO_24, GPIO_27) "
                      "VALUES (:date, :GPIO_18_val, :GPIO_22_val, :GPIO_24_val, :GPIO_27_val)");
        query.bindValue(":date", QDateTime::currentDateTime().toTime_t());
        query.bindValue(":GPIO_18_val", this->GPIO_18_val);
        query.bindValue(":GPIO_22_val", this->GPIO_22_val);
        query.bindValue(":GPIO_24_val", this->GPIO_24_val);
        query.bindValue(":GPIO_27_val", this->GPIO_27_val);
        bool ret = query.exec();
        if (ret)
                    {
                    // http://www.sqlite.org/c3ref/last_insert_rowid.html
                    int newID = query.lastInsertId().toInt();
                    //qDebug() << "insert success for ID:" << newID;
                    }
        else
        {
            qDebug() << "error inserting gpio settings into db";
            qDebug() << this->system_db->lastError();
        }
  //  }
  //  else
  //  {
  //      qDebug() << "update_GPIO_Settings could not open database"
  //  }
}

void MainWindow::update_HLT_PID_Settings()
{
    // correct for temp sensor calibration
    double PID_set_temp_corrected = this->HLT_PID_setpoint - this->TempProbeCalibrationPopup_Window.HLT_TempCorrection;
    //if (this->system_db->isOpen())
    //{
       // qDebug() << "db is open, trying to insert...";
       // qDebug() << this->system_db->tables();
        QSqlQuery query(*this->system_db);
        query.prepare("INSERT INTO HLT_pid_settings (date, PID_set_temp, PID_P_val, PID_I_val, PID_D_val, ENABLE_bool) "
                      "VALUES (:date, :PID_set_temp, :PID_P_val, :PID_I_val, :PID_D_val, :ENABLE_bool)");
        query.bindValue(":date", QDateTime::currentDateTime().toTime_t());
        //query.bindValue(":PID_set_temp", this->HLT_PID_setpoint);
        query.bindValue(":PID_set_temp", PID_set_temp_corrected);
        query.bindValue(":PID_P_val", this->HLT_PID_P_val);
        query.bindValue(":PID_I_val", this->HLT_PID_I_val);
        query.bindValue(":PID_D_val", this->HLT_PID_D_val);
        query.bindValue(":ENABLE_bool", this->HLT_PID_enable);
        bool ret = query.exec();
        if (ret)
                    {
                    // http://www.sqlite.org/c3ref/last_insert_rowid.html
                    int newID = query.lastInsertId().toInt();
                    //qDebug() << "insert success for ID:" << newID;
                    }
        else
        {
            qDebug() << "error inserting HLT PID settings into db";
            qDebug() << this->system_db->lastError();
        }
    //}
    //else
    //{
    //    qDebug() << "update_GPIO_Settings could not open database"
    //}
    // if HLT PID is enabled, automatically turn on stir motor (to avoid temp stratification) - still turns on if interlock flags, which is fine
        if(this->HLT_PID_enable)
        {
            ui->r_StirMotor_ON->setChecked(true);
            this->on_r_StirMotor_ON_clicked();
        }
}

void MainWindow::update_BK_PID_Settings()
{
    // correct for temp sensor calibration
    double PID_set_temp_corrected = this->BK_PID_setpoint - this->TempProbeCalibrationPopup_Window.BK_TempCorrection;
    QSqlQuery query(*this->system_db);
    query.prepare("INSERT INTO BK_pid_settings (date, PID_set_temp, PID_P_val, PID_I_val, PID_D_val, ENABLE_bool) "
                  "VALUES (:date, :PID_set_temp, :PID_P_val, :PID_I_val, :PID_D_val, :ENABLE_bool)");
    query.bindValue(":date", QDateTime::currentDateTime().toTime_t());
    query.bindValue(":PID_set_temp", PID_set_temp_corrected);
    query.bindValue(":PID_P_val", this->BK_PID_P_val);
    query.bindValue(":PID_I_val", this->BK_PID_I_val);
    query.bindValue(":PID_D_val", this->BK_PID_D_val);
    query.bindValue(":ENABLE_bool", this->BK_PID_enable);
        bool ret = query.exec();
        if (ret)
                    {
                    // http://www.sqlite.org/c3ref/last_insert_rowid.html
                    int newID = query.lastInsertId().toInt();
                    //qDebug() << "insert success for ID:" << newID;
                    }
        else
        {
            qDebug() << "error inserting BK PID settings into db";
            qDebug() << this->system_db->lastError();
        }
}

/*void MainWindow::on_LE_Recipe_MashDuration_editingFinished()
{
    int hours = ui->LE_Recipe_MashDuration->text().toInt()/60;
    int minutes = ui->LE_Recipe_MashDuration->text().toInt()-(hours*60);
    QTime timer = QTime(hours,minutes,0,0);
    ui->L_MLT_MashTimer->setText(timer.toString());
}*/

/*void MainWindow::on_DSB_Config_VorlaufTime_editingFinished()
{
    //int minutes = ui->DSB_Config_VorlaufTime->value();
    int minutes = this->BrewConstantsPopup_Window.default_vorlauf_time;
    QTime timer = QTime(0,minutes,0,0);
    ui->L_MLT_VorlaufTimer->setText(timer.toString());
}*/

void MainWindow::on_PB_HLT_UpdatePID_clicked()
{
    this->HLT_PID_setpoint = ui->LE_HLT_PID_SetTemp->text().toDouble();
    //this->HLT_PID_P_val = ui->LE_HLT_PID_PVal->text().toDouble();
    //this->HLT_PID_I_val = ui->LE_HLT_PID_IVal->text().toDouble();
    //this->HLT_PID_D_val = ui->LE_HLT_PID_DVal->text().toDouble();
    update_HLT_PID_Settings();
    ui->L_HLT_SetTemp->setText(QString::number(this->HLT_PID_setpoint));
    ui->L_HLT_SetTemp_Main->setText(QString::number(this->HLT_PID_setpoint));
}

/*void MainWindow::on_PB_HLT_CaptureGroundTemp_clicked()
{
    //ui->DSB_Config_GroundWaterTemp->setValue(ui->L_HLT_CurrentTemp->text().toDouble());
    ui->L_HLT_GroundWaterTemp->setText(ui->L_HLT_CurrentTemp->text());

    qDebug() << "inserting brewday_log entry: groundwater temp: " << ui->L_HLT_GroundWaterTemp->text();

    QSqlQuery query(*this->system_db);
    query.prepare("INSERT INTO brewday_log (GroundWaterTemp)"
                  "VALUES (:groundwater_temp)");
    query.bindValue(":groundwater_temp", ui->L_HLT_GroundWaterTemp->text().toDouble());

    bool ret = query.exec();
    if (ret)
                {
                // http://www.sqlite.org/c3ref/last_insert_rowid.html
                int newID = query.lastInsertId().toInt();
                qDebug() << "insert success for ID:" << newID;
                }
    else
    {
        qDebug() << "errrorrrr";
        qDebug() << this->system_db->lastError();
    }
}*/

void MainWindow::on_PB_Timer1_Set_clicked()
{
    QTime qtime_remaining = QTime(ui->SB_Timer1_Hours->value(),ui->SB_Timer1_Minutes->value(),0,0);
    ui->L_Timer1->setText(qtime_remaining.toString());
}

void MainWindow::on_PB_Timer1_Start_clicked()
{
    this->Timer1_StartTime = new QDateTime(QDateTime::currentDateTime());
    this->Timer1_EndTime = new QDateTime(this->Timer1_StartTime->addSecs(ui->SB_Timer1_Hours->text().toInt()*60*60+ui->SB_Timer1_Minutes->text().toInt()*60));
    connect(this->timer1_timer, SIGNAL(timeout()), this, SLOT(updateTimer1Display()));
    this->timer1_timer->start(1000);
}

void MainWindow::on_PB_Timer1_Stop_clicked()
{
    this->timer1_timer->stop();
}

void MainWindow::on_PB_Timer2_Set_clicked()
{
    QTime qtime_remaining = QTime(ui->SB_Timer2_Hours->value(),ui->SB_Timer2_Minutes->value(),0,0);
    ui->L_Timer2->setText(qtime_remaining.toString());
}

void MainWindow::on_PB_Timer2_Start_clicked()
{
    this->Timer2_StartTime = new QDateTime(QDateTime::currentDateTime());
    this->Timer2_EndTime = new QDateTime(this->Timer2_StartTime->addSecs(ui->SB_Timer2_Hours->text().toInt()*60*60+ui->SB_Timer2_Minutes->text().toInt()*60));
    connect(this->timer2_timer, SIGNAL(timeout()), this, SLOT(updateTimer2Display()));
    this->timer2_timer->start(1000);
}

void MainWindow::on_PB_Timer2_Stop_clicked()
{
    this->timer2_timer->stop();
}

void MainWindow::on_PB_Timer3_Set_clicked()
{
    QTime qtime_remaining = QTime(ui->SB_Timer3_Hours->value(),ui->SB_Timer3_Minutes->value(),0,0);
    ui->L_Timer3->setText(qtime_remaining.toString());
}

void MainWindow::on_PB_Timer3_Start_clicked()
{
    this->Timer3_StartTime = new QDateTime(QDateTime::currentDateTime());
    this->Timer3_EndTime = new QDateTime(this->Timer3_StartTime->addSecs(ui->SB_Timer3_Hours->text().toInt()*60*60+ui->SB_Timer3_Minutes->text().toInt()*60));
    connect(this->timer3_timer, SIGNAL(timeout()), this, SLOT(updateTimer3Display()));
    this->timer3_timer->start(1000);
}

void MainWindow::on_PB_Timer3_Stop_clicked()
{
    this->timer3_timer->stop();
}

void MainWindow::on_PB_Timer4_Set_clicked()
{
    QTime qtime_remaining = QTime(ui->SB_Timer4_Hours->value(),ui->SB_Timer4_Minutes->value(),0,0);
    ui->L_Timer4->setText(qtime_remaining.toString());
}

void MainWindow::on_PB_Timer4_Start_clicked()
{
    this->Timer4_StartTime = new QDateTime(QDateTime::currentDateTime());
    this->Timer4_EndTime = new QDateTime(this->Timer4_StartTime->addSecs(ui->SB_Timer4_Hours->text().toInt()*60*60+ui->SB_Timer4_Minutes->text().toInt()*60));
    connect(this->timer4_timer, SIGNAL(timeout()), this, SLOT(updateTimer4Display()));
    this->timer4_timer->start(1000);
}

void MainWindow::on_PB_Timer4_Stop_clicked()
{
    this->timer4_timer->stop();
}

/*void MainWindow::on_DSB_Config_TempDropHLTtoHERMS_editingFinished()
{

}*/

void MainWindow::on_CB_HLT_TimeRangeScale_currentIndexChanged(int index)
{
    //qDebug() << "index: " << index;
    switch(index)
    {
        case 5:
            this->hlt_range_ticks = 2 * 60 * 60;
            ui->customPlot_HLT->xAxis->setTickStep(5);
            //qDebug() << "changing to 2 hour range scale";
            break;
        case 4:
            this->hlt_range_ticks = 60 * 60;
            ui->customPlot_HLT->xAxis->setTickStep(10);
            //qDebug() << "changing to 1 hour range scale";
            break;
        case 3:
            this->hlt_range_ticks = 30 * 60;
            ui->customPlot_HLT->xAxis->setTickStep(20);
            //qDebug() << "changing to 30 min range scale";
            break;
        case 2:
            this->hlt_range_ticks = 15 * 60;
            ui->customPlot_HLT->xAxis->setTickStep(50);
            //qDebug() << "changing to 15 min range scale";
            break;
        case 1:
            this->hlt_range_ticks = 5 * 60;
            ui->customPlot_HLT->xAxis->setTickStep(100);
            //qDebug() << "changing to 5 min range scale";
            break;
        case 0:
            this->hlt_range_ticks = 60;
            ui->customPlot_HLT->xAxis->setTickStep(60);
            //qDebug() << "changing to 1 min range scale";
            break;
        default:
            break;
    }
    ui->customPlot_HLT->replot();
}

void MainWindow::on_CB_MLT_TimeRangeScale_currentIndexChanged(int index)
{
    //qDebug() << "index: " << index;
    switch(index)
    {
        case 5:
            this->mlt_range_ticks = 2 * 60 * 60;
            ui->customPlot_MLT->xAxis->setTickStep(5);
            //qDebug() << "changing to 2 hour range scale";
            break;
        case 4:
            this->mlt_range_ticks = 60 * 60;
            ui->customPlot_MLT->xAxis->setTickStep(10);
            //qDebug() << "changing to 1 hour range scale";
            break;
        case 3:
            this->mlt_range_ticks = 30 * 60;
            ui->customPlot_MLT->xAxis->setTickStep(20);
            //qDebug() << "changing to 30 min range scale";
            break;
        case 2:
            this->mlt_range_ticks = 15 * 60;
            ui->customPlot_MLT->xAxis->setTickStep(50);
            //qDebug() << "changing to 15 min range scale";
            break;
        case 1:
            this->mlt_range_ticks = 5 * 60;
            ui->customPlot_MLT->xAxis->setTickStep(100);
            //qDebug() << "changing to 5 min range scale";
            break;
        case 0:
            this->mlt_range_ticks = 60;
            ui->customPlot_MLT->xAxis->setTickStep(60);
            //qDebug() << "changing to 1 min range scale";
            break;
        default:
            break;
    }
    ui->customPlot_MLT->replot();
}

void MainWindow::on_CB_BK_TimeRangeScale_currentIndexChanged(int index)
{
    //qDebug() << "index: " << index;
    switch(index)
    {
        case 5:
            this->bk_range_ticks = 2 * 60 * 60;
            ui->customPlot_BK->xAxis->setTickStep(5);
            //qDebug() << "changing to 2 hour range scale";
            break;
        case 4:
            this->bk_range_ticks = 60 * 60;
            ui->customPlot_BK->xAxis->setTickStep(10);
            //qDebug() << "changing to 1 hour range scale";
            break;
        case 3:
            this->bk_range_ticks = 30 * 60;
            ui->customPlot_BK->xAxis->setTickStep(20);
            //qDebug() << "changing to 30 min range scale";
            break;
        case 2:
            this->bk_range_ticks = 15 * 60;
            ui->customPlot_BK->xAxis->setTickStep(50);
            //qDebug() << "changing to 15 min range scale";
            break;
        case 1:
            this->bk_range_ticks = 5 * 60;
            ui->customPlot_BK->xAxis->setTickStep(100);
            //qDebug() << "changing to 5 min range scale";
            break;
        case 0:
            this->bk_range_ticks = 60;
            ui->customPlot_BK->xAxis->setTickStep(60);
            //qDebug() << "changing to 1 min range scale";
            break;
        default:
            break;
    }
    ui->customPlot_BK->replot();
}

void MainWindow::on_CB_Chill_TimeRangeScale_currentIndexChanged(int index)
{
    //qDebug() << "index: " << index;
    switch(index)
    {
        case 5:
            this->chill_range_ticks = 2 * 60 * 60;
            ui->customPlot_Chill->xAxis->setTickStep(5);
            //qDebug() << "changing to 2 hour range scale";
            break;
        case 4:
            this->chill_range_ticks = 60 * 60;
            ui->customPlot_Chill->xAxis->setTickStep(10);
            //qDebug() << "changing to 1 hour range scale";
            break;
        case 3:
            this->chill_range_ticks = 30 * 60;
            ui->customPlot_Chill->xAxis->setTickStep(20);
            //qDebug() << "changing to 30 min range scale";
            break;
        case 2:
            this->chill_range_ticks = 15 * 60;
            ui->customPlot_Chill->xAxis->setTickStep(50);
            //qDebug() << "changing to 15 min range scale";
            break;
        case 1:
            this->chill_range_ticks = 5 * 60;
            ui->customPlot_Chill->xAxis->setTickStep(100);
            //qDebug() << "changing to 5 min range scale";
            break;
        case 0:
            this->chill_range_ticks = 60;
            ui->customPlot_Chill->xAxis->setTickStep(60);
            //qDebug() << "changing to 1 min range scale";
            break;
        default:
            break;
    }
    ui->customPlot_Chill->replot();
}

void MainWindow::on_PB_MLT_UpdateHLTPIDSetTemp_clicked()
{
    double step_temp = ui->DSB_MLT_SetMashTemp->value();
    ui->L_MLT_SetTemp->setText(QString::number(step_temp));
    ui->L_MLT_SetTemp_Main->setText(QString::number(step_temp));

    // set HLT PID to next step temp plus the drop between the HLT and the HERMS coil plus the temp delta between HERMS output and MLT
    this->HLT_PID_setpoint = step_temp + this->TempProbeCalibrationPopup_Window.HERMS_TempCorrection + this->BrewConstantsPopup_Window.temp_drop_from_hlt_to_herms_out + this->BrewConstantsPopup_Window.temp_drop_from_herms_out_to_mlt_in;
    this->update_HLT_PID_Settings();
    ui->L_HLT_SetTemp->setText(QString::number(this->HLT_PID_setpoint));
    ui->L_HLT_SetTemp_Main->setText(QString::number(this->HLT_PID_setpoint));
    ui->LE_HLT_PID_SetTemp->setText(QString::number(this->HLT_PID_setpoint));
}

void MainWindow::on_r_StirMotor_ON_clicked()
{
    if (this->GPIO_22_val != 1)
    {
        this->GPIO_22_val = 1;
        update_GPIO_Settings();
    }
    else
    {
        //qDebug() << "stir motor already on!";
    }
}

void MainWindow::on_r_StirMotor_OFF_clicked()
{
    if (this->GPIO_22_val != 0)
    {
        this->GPIO_22_val = 0;
        update_GPIO_Settings();
    }
    else
    {
        //qDebug() << "stir motor already off!";
    }
}

void MainWindow::on_LE_Recipe_PitchTemp_editingFinished()
{
    ui->L_Chill_YeastPitchTemp->setText(ui->LE_Recipe_PitchTemp->text());
}

/*void MainWindow::on_Table_Config_TempProbeCalibration_cellChanged(int row, int column)
{
    // check if it was the correction column
    if(column == 0)
    {
        switch(row)
        {
            case 0:
                this->HLT_Temp_Correction = ui->Table_Config_TempProbeCalibration->item(row,column)->text().toDouble();
                break;
            case 1:
                this->HERMS_Temp_Correction = ui->Table_Config_TempProbeCalibration->item(row,column)->text().toDouble();
                break;
            case 2:
                this->BK_Temp_Correction = ui->Table_Config_TempProbeCalibration->item(row,column)->text().toDouble();
                break;
            case 3:
                this->Chill_Temp_Correction = ui->Table_Config_TempProbeCalibration->item(row,column)->text().toDouble();
                break;
            default:
                break;
        }
    }
}*/

void MainWindow::on_HLT_Auto_Scale_Enable_ChkBox_clicked()
{
    // rescale value (vertical) axis to fit the current data:
    if(ui->HLT_Auto_Scale_Enable_ChkBox->isChecked())
    {
        ui->customPlot_HLT->graph(0)->rescaleValueAxis();
        //ui->customPlot_HLT->graph(1)->rescaleValueAxis();
    }
    else
        ui->customPlot_HLT->yAxis->setRange(50, 200);
    // update plot
    ui->customPlot_HLT->replot();
}

void MainWindow::on_CB_Logs_TimeRangeScale_currentIndexChanged(int index)
{
    //qDebug() << "index: " << index;
    switch(index)
    {
        case 5:
            this->logs_range_ticks = 2 * 60 * 60;
            ui->customPlot_Logs->xAxis->setTickStep(5);
            //qDebug() << "changing to 2 hour range scale";
            break;
        case 4:
            this->logs_range_ticks = 60 * 60;
            ui->customPlot_Logs->xAxis->setTickStep(10);
            //qDebug() << "changing to 1 hour range scale";
            break;
        case 3:
            this->logs_range_ticks = 30 * 60;
            ui->customPlot_Logs->xAxis->setTickStep(20);
            //qDebug() << "changing to 30 min range scale";
            break;
        case 2:
            this->logs_range_ticks = 15 * 60;
            ui->customPlot_Logs->xAxis->setTickStep(50);
            //qDebug() << "changing to 15 min range scale";
            break;
        case 1:
            this->logs_range_ticks = 5 * 60;
            ui->customPlot_Logs->xAxis->setTickStep(100);
            //qDebug() << "changing to 5 min range scale";
            break;
        case 0:
            this->logs_range_ticks = 60;
            ui->customPlot_Logs->xAxis->setTickStep(60);
            //qDebug() << "changing to 1 min range scale";
            break;
        default:
            break;
    }
    ui->customPlot_Logs->replot();
}

void MainWindow::on_Logs_Auto_Scale_Enable_ChkBox_clicked()
{
    // rescale value (vertical) axis to fit the current data:
    if(ui->Logs_Auto_Scale_Enable_ChkBox->isChecked())
    {
        ui->customPlot_Logs->graph(0)->rescaleValueAxis();
        //ui->customPlot_Logs->graph(1)->rescaleValueAxis();
        ui->customPlot_Logs->graph(2)->rescaleValueAxis();
        ui->customPlot_Logs->graph(3)->rescaleValueAxis();
        //ui->customPlot_Logs->graph(4)->rescaleValueAxis();
        ui->customPlot_Logs->graph(5)->rescaleValueAxis();
    }
    else
        ui->customPlot_Logs->yAxis->setRange(50, 200);
    // update plot
    ui->customPlot_Logs->replot();
}

void MainWindow::on_MLT_Auto_Scale_Enable_ChkBox_clicked()
{
    // rescale value (vertical) axis to fit the current data:
    if(ui->MLT_Auto_Scale_Enable_ChkBox->isChecked())
    {
        ui->customPlot_MLT->graph(0)->rescaleValueAxis();
        ui->customPlot_MLT->graph(1)->rescaleValueAxis();
    }
    else
        ui->customPlot_MLT->yAxis->setRange(50, 200);
    // update plot
    ui->customPlot_MLT->replot();
}

void MainWindow::on_Boil_Auto_Scale_Enable_ChkBox_clicked()
{
    // rescale value (vertical) axis to fit the current data:
    if(ui->Boil_Auto_Scale_Enable_ChkBox->isChecked())
    {
        ui->customPlot_BK->graph(0)->rescaleValueAxis();
        //ui->customPlot_BK->graph(1)->rescaleValueAxis();
    }
    else
        ui->customPlot_BK->yAxis->setRange(50, 200);
    // update plot
    ui->customPlot_BK->replot();
}

void MainWindow::on_Chill_Auto_Scale_Enable_ChkBox_clicked()
{
    // rescale value (vertical) axis to fit the current data:
    if(ui->Chill_Auto_Scale_Enable_ChkBox->isChecked())
    {
        ui->customPlot_Chill->graph(0)->rescaleValueAxis();
        ui->customPlot_Chill->graph(1)->rescaleValueAxis();
    }
    else
        ui->customPlot_Chill->yAxis->setRange(50, 200);
    // update plot
    ui->customPlot_Chill->replot();
}

void MainWindow::on_PB_Logs_Browse_SaveLogAs_clicked()
{
    this->CSV_fileName = QFileDialog::getSaveFileName(this,
        tr("Save Data"), "/home/pi/"+this->CSV_fileName, tr("Comma Separated Values (*.csv)"));

    ui->LE_Logs_SaveLogAs->setText(this->CSV_fileName);
}

void MainWindow::on_PB_Logs_Save_SaveLogAs_clicked()
{
    // Convert database to csv specified in filename dialog
    this->CSV_fileName.chop(4);
    this->export_db_table("HLT_pid_settings");
    this->export_db_table("HLT_current_pid_data");
    this->export_db_table("mash_temp_sensor_data");
    this->export_db_table("boil_temp_sensor_data");
    this->export_db_table("chill_temp_sensor_data");
    this->export_db_table("gpio_outputs");
    this->export_db_table("checklist_times");
    this->export_db_table("brewday_log");
}

void MainWindow::export_db_table(QString table_name)
{

    QString CSV_file_to_create = this->CSV_fileName + "_" + table_name + ".csv";
    // Convert database to csv specified in filename dialog
    QString command = "/bin/sh -c \"echo \"\".headers on\"\" > /tmp/sqlinput.txt\"";
    QProcess::execute(command.toUtf8().constData());
    command = "/bin/sh -c \"echo \"\".mode csv\"\" >> /tmp/sqlinput.txt\"";
    QProcess::execute(command.toUtf8().constData());
    command = "/bin/sh -c \"echo \"\".output "+CSV_file_to_create+"\"\" >> /tmp/sqlinput.txt\"";
    QProcess::execute(command.toUtf8().constData());
    QFile file("/tmp/sqlinput.txt");
    file.open(QIODevice::WriteOnly| QIODevice::Append );
    QString command_text = "select * from " + table_name + ";";
    file.write(command_text.toUtf8().constData());
    file.close();
    command = "/bin/sh -c \"sqlite3 /DB/brewing_db1.db < /tmp/sqlinput.txt\"";
    QProcess::execute(command.toUtf8().constData());
}

void MainWindow::on_HLT_Temp_Enable_ChkBox_HLT_clicked()
{
    if(ui->HLT_Temp_Enable_ChkBox_HLT->isChecked())
        ui->customPlot_HLT->graph(0)->setVisible(true);
    else
        ui->customPlot_HLT->graph(0)->setVisible(false);
    ui->customPlot_HLT->replot();
}

void MainWindow::on_HLT_PWM_Enable_ChkBox_clicked()
{
    if(ui->HLT_PWM_Enable_ChkBox->isChecked())
        ui->customPlot_HLT->graph(1)->setVisible(true);
    else
        ui->customPlot_HLT->graph(1)->setVisible(false);
    ui->customPlot_HLT->replot();
}

void MainWindow::on_MLT_Temp_Enable_ChkBox_HERMS_clicked()
{
    if(ui->MLT_Temp_Enable_ChkBox_HERMS->isChecked())
        ui->customPlot_MLT->graph(0)->setVisible(true);
    else
        ui->customPlot_MLT->graph(0)->setVisible(false);
    ui->customPlot_MLT->replot();
}

void MainWindow::on_MLT_Temp_Enable_ChkBox_HLT_clicked()
{
    if(ui->MLT_Temp_Enable_ChkBox_HLT->isChecked())
        ui->customPlot_MLT->graph(1)->setVisible(true);
    else
        ui->customPlot_MLT->graph(1)->setVisible(false);
    ui->customPlot_MLT->replot();
}

void MainWindow::on_Boil_Temp_Enable_ChkBox_Boil_clicked()
{
    if(ui->Boil_Temp_Enable_ChkBox_Boil->isChecked())
        ui->customPlot_BK->graph(0)->setVisible(true);
    else
        ui->customPlot_BK->graph(0)->setVisible(false);
    ui->customPlot_BK->replot();
}

void MainWindow::on_Boil_PWM_Enable_ChkBox_clicked()
{
    if(ui->Boil_PWM_Enable_ChkBox->isChecked())
        ui->customPlot_BK->graph(1)->setVisible(true);
    else
        ui->customPlot_BK->graph(1)->setVisible(false);
    ui->customPlot_BK->replot();
}

void MainWindow::on_Chill_Temp_Enable_ChkBox_Chill_clicked()
{
    if(ui->Chill_Temp_Enable_ChkBox_Chill->isChecked())
        ui->customPlot_Chill->graph(0)->setVisible(true);
    else
        ui->customPlot_Chill->graph(0)->setVisible(false);
    ui->customPlot_Chill->replot();
}

void MainWindow::on_Chill_Temp_Enable_ChkBox_Boil_clicked()
{
    if(ui->Chill_Temp_Enable_ChkBox_Boil->isChecked())
        ui->customPlot_Chill->graph(1)->setVisible(true);
    else
        ui->customPlot_Chill->graph(1)->setVisible(false);
    ui->customPlot_Chill->replot();
}

void MainWindow::on_Logs_Temp_Enable_ChkBox_HLT_clicked()
{
    if(ui->Logs_Temp_Enable_ChkBox_HLT->isChecked())
        ui->customPlot_Logs->graph(0)->setVisible(true);
    else
        ui->customPlot_Logs->graph(0)->setVisible(false);
    ui->customPlot_Logs->replot();
}

void MainWindow::on_Logs_PWM_Enable_ChkBox_clicked()
{
    if(ui->Logs_PWM_Enable_ChkBox->isChecked())
        ui->customPlot_Logs->graph(1)->setVisible(true);
    else
        ui->customPlot_Logs->graph(1)->setVisible(false);
    ui->customPlot_Logs->replot();
}

void MainWindow::on_Logs_Temp_Enable_ChkBox_HERMS_clicked()
{
    if(ui->Logs_Temp_Enable_ChkBox_HERMS->isChecked())
        ui->customPlot_Logs->graph(2)->setVisible(true);
    else
        ui->customPlot_Logs->graph(2)->setVisible(false);
    ui->customPlot_Logs->replot();
}

void MainWindow::on_Logs_Temp_Enable_ChkBox_Boil_clicked()
{
    if(ui->Logs_Temp_Enable_ChkBox_Boil->isChecked())
        ui->customPlot_Logs->graph(3)->setVisible(true);
    else
        ui->customPlot_Logs->graph(3)->setVisible(false);
    ui->customPlot_Logs->replot();
}

void MainWindow::on_Logs_PWM_Enable_ChkBox_2_clicked()
{
    if(ui->Logs_PWM_Enable_ChkBox_2->isChecked())
        ui->customPlot_Logs->graph(4)->setVisible(true);
    else
        ui->customPlot_Logs->graph(4)->setVisible(false);
    ui->customPlot_Logs->replot();
}

void MainWindow::on_Logs_Temp_Enable_ChkBox_Chill_clicked()
{
    if(ui->Logs_Temp_Enable_ChkBox_Chill->isChecked())
        ui->customPlot_Logs->graph(5)->setVisible(true);
    else
        ui->customPlot_Logs->graph(5)->setVisible(false);
    ui->customPlot_Logs->replot();
}

void MainWindow::soundAlarm(QString alarmText)
{
    QString command = "omxplayer "+this->Alarm_SoundFile + "&";
    //QString command = "aplay "+this->Alarm_SoundFile +"&";
    system(command.toUtf8().constData());
    ui->PB_AlarmText->setText(alarmText);
    ui->PB_AlarmText->setStyleSheet("QPushButton { color : red; }");
}

/*void MainWindow::on_PB_Config_AlarmBrowse_clicked()
{
    this->Alarm_SoundFile = QFileDialog::getOpenFileName(this,
        tr("Alarm Sound File"), "/home/pi/"+this->Alarm_SoundFile, tr("All Files (*.*)"));
    ui->LE_Config_AlarmSoundFile->setText(this->Alarm_SoundFile);
}*/

void MainWindow::on_PB_AlarmText_clicked()
{
    ui->PB_AlarmText->setText("NONE");
    ui->PB_AlarmText->setStyleSheet("QPushButton { color : green; }");
}

void MainWindow::on_PB_Recipe_ImportBeerXMLRecipe_Browse_clicked()
{
    ui->CB_MLT_MashHopAddition->setEnabled(false);
    ui->CB_MLT_MashHopAddition->setVisible(false);
    this->BeerXML_Recipe_filename = QFileDialog::getOpenFileName(this,
        tr("BeerXML Recipe"), "/home/pi/"+this->BeerXML_Recipe_filename, tr("XML Files (*.XML)"));
    ui->LE_Recipe_ImportBeerXML_Path->setText(this->BeerXML_Recipe_filename);
    this->recipe = new BeerXMLRecipe(this->BeerXML_Recipe_filename.toStdString().c_str());
    // populate recipe page with parsed data
    ui->LE_Recipe_Name->setText(recipe->recipe_name);
    ui->LE_Recipe_BrewType->setText(recipe->recipe_type);
    ui->LE_Recipe_BrewerName->setText(recipe->brewer_name);
    ui->LE_Recipe_BatchSize->setText(QString::number(this->LtoGal(recipe->batch_size), 'f', 1));
    ui->LE_Recipe_BoilSize->setText(QString::number(this->LtoGal(recipe->boil_size), 'f', 1));
    ui->LE_Recipe_BoilTime->setText(QString::number(recipe->boil_time));
    ui->LE_Recipe_Efficiency->setText(QString::number(recipe->efficiency));

    // go thru mash steps vector and populate table
    for (unsigned int i=0; i< recipe->mash_steps.size(); i++)
    {
        ui->LE_Recipe_AddStep_Name->setText(recipe->mash_steps[i].getStepName());
        ui->LE_Recipe_AddStep_Temperature->setText(QString::number(this->CtoF(recipe->mash_steps[i].getStepTemperature())));
        ui->LE_Recipe_AddStep_Time->setText(QString::number(recipe->mash_steps[i].getStepLength()));
        this->on_PB_Recipe_AddStep_clicked();
    }

    // go thru hop additions vector and populate table
    for (unsigned int i=0;i<recipe->hop_additions.size();i++)
    {
        // ignore dry hop additions for this part
        if(recipe->hop_additions[i].use != "Dry Hop")
        {
            ui->LE_Recipe_AddHop_Amount->setText(recipe->hop_additions[i].display_amount);
            ui->LE_Recipe_AddHop_Name->setText(recipe->hop_additions[i].name);
            ui->LE_Recipe_AddHop_Time->setText(QString::number(recipe->hop_additions[i].time));
            ui->LE_Recipe_AddHop_Type->setText(recipe->hop_additions[i].type);
            ui->LE_Recipe_AddHop_Use->setText(recipe->hop_additions[i].use);
            // special case for Mash Hopping, add info to MLT page
            if(recipe->hop_additions[i].use == "Mash")
            {
                ui->CB_MLT_MashHopAddition->setVisible(true);
                ui->CB_MLT_MashHopAddition->setEnabled(true);
                ui->CB_MLT_MashHopAddition->setText("Mash Hop: " + recipe->hop_additions[i].display_amount + " " + recipe->hop_additions[i].name);
            }
            // special case for First Wort Hopping, add info to MLT page
            if(recipe->hop_additions[i].use == "First Wort")
            {
                ui->CB_MLT_FWHopAddition->setVisible(true);
                ui->CB_MLT_FWHopAddition->setEnabled(true);
                ui->CB_MLT_FWHopAddition->setText("First Wort Hop: " + recipe->hop_additions[i].display_amount + " " + recipe->hop_additions[i].name);
            }
            this->on_PB_Recipe_AddHop_clicked();
        }
    }

    ui->LE_Recipe_PitchTemp->setText(QString::number(this->CtoF(recipe->pitch_temp)));
    ui->L_Chill_YeastPitchTemp->setText(QString::number(this->CtoF(recipe->pitch_temp)));

    // go thru fermentables vector and populate table
    for (unsigned int i=0;i<recipe->fermentables.size();i++)
    {
            ui->LE_Recipe_AddFermentable_Name->setText(recipe->fermentables[i].name);
            ui->LE_Recipe_AddFermentable_Amount->setText(QString::number(this->KGtoLB(recipe->fermentables[i].amount)));
            ui->LE_Recipe_AddFermentable_Color->setText(recipe->fermentables[i].display_color);
            this->on_PB_Recipe_AddFermentable_clicked();
    }

    ui->LE_Recipe_Style->setText(recipe->style_name);
    ui->LE_Recipe_YeastName->setText(recipe->yeast_name);
    ui->LE_Recipe_StrikeTemp->setText(QString::number(recipe->strike_temp));
    ui->L_Recipe_MashInDescription->setText(recipe->strike_description);
    ui->L_Recipe_GrainWeight->setText(QString::number(recipe->grain_weight, 'f', 1));

    // calculate Total & Sparge Volume based on Recipe and Brew Constants
    double shrinkage_factor = 1-(this->BrewConstantsPopup_Window.wort_shrinkage_factor/100);
    double boiloff_amount = this->BrewConstantsPopup_Window.boiloff_rate*recipe->boil_time/60;
    double totalWaterVolume =
            ((LtoGal(recipe->batch_size) + this->BrewConstantsPopup_Window.brew_kettle_trub_loss)
            / shrinkage_factor
            + boiloff_amount
            + this->BrewConstantsPopup_Window.mash_tun_loss
            + (this->BrewConstantsPopup_Window.grain_absorption * recipe->grain_weight))
            * 4;
    ui->L_Recipe_TotalWater->setText(QString::number(totalWaterVolume, 'f', 1));
    double mash_water = recipe->mash_thickness * recipe->grain_weight + this->BrewConstantsPopup_Window.mash_tun_loss*4;
    ui->LE_Recipe_MashThickness->setText(QString::number(recipe->mash_thickness));
    //qDebug()<< "mash water: " << mash_water;
    double postboil_volume = (LtoGal(recipe->batch_size) + this->BrewConstantsPopup_Window.brew_kettle_trub_loss)/ shrinkage_factor;
    ui->L_Recipe_PostBoilVol->setText(QString::number(postboil_volume, 'f', 1));
    double preboil_volume = postboil_volume + boiloff_amount;
    //qDebug()<< "postboil volume: " << postboil_volume;
    //qDebug()<< "preboil volume: " << preboil_volume;
    double sparge_water = totalWaterVolume - mash_water;
    ui->L_Recipe_SpargeWater->setText(QString::number(sparge_water, 'f', 1));
    ui->CB_MLT_SpargeAmount->setText("Sparge with " + QString::number(sparge_water, 'f', 1) + " quarts water");
    ui->CB_MLT_SpargeAmount->setEnabled(true);
}

double MainWindow::CtoF(double celsius)
{
    double fahrenheit = celsius*9/5 + 32;
    return fahrenheit;
}

double MainWindow::LtoGal(double liters)
{
    double gallons = liters*0.26417053;
    return gallons;
}

double MainWindow::KGtoLB(double kilograms)
{
    double pounds = kilograms * 2.20462442;
    return pounds;
}

void MainWindow::on_PB_Recipe_AddFermentable_clicked()
{
    int numRows_config = ui->table_Recipe_Fermentables->rowCount();

    ui->table_Recipe_Fermentables->insertRow(numRows_config);
    ui->table_Recipe_Fermentables->setItem(numRows_config,0, new QTableWidgetItem(ui->LE_Recipe_AddFermentable_Name->text()));
    ui->table_Recipe_Fermentables->setItem(numRows_config,1, new QTableWidgetItem(ui->LE_Recipe_AddFermentable_Amount->text()));
    ui->table_Recipe_Fermentables->setItem(numRows_config,2, new QTableWidgetItem(ui->LE_Recipe_AddFermentable_Color->text()));

    ui->L_Recipe_GrainWeight->setText(QString::number(ui->L_Recipe_GrainWeight->text().toDouble() + ui->LE_Recipe_AddFermentable_Amount->text().toDouble()));

    // clear line_edits
    ui->LE_Recipe_AddFermentable_Name->setText("");
    ui->LE_Recipe_AddFermentable_Amount->setText("");
    ui->LE_Recipe_AddFermentable_Color->setText("");
}

void MainWindow::on_LE_Logs_PreBoilGravity_editingFinished()
{
    qDebug() << "inserting brewday_log entry: Preboil gravity: " << ui->LE_Logs_PreBoilGravity->text();

    QSqlQuery query(*this->system_db);
    query.prepare("INSERT INTO brewday_log (PreBoilGravity)"
                  "VALUES (:preboil_grav)");
    query.bindValue(":preboil_grav", ui->LE_Logs_PreBoilGravity->text());

    bool ret = query.exec();
    if (ret)
                {
                // http://www.sqlite.org/c3ref/last_insert_rowid.html
                int newID = query.lastInsertId().toInt();
                //qDebug() << "insert success for ID:" << newID;
                }
    else
    {
        qDebug() << "error inserting preboil grav into db";
        qDebug() << this->system_db->lastError();
    }
}

void MainWindow::on_PB_BK_SetForMashIn_clicked()
{
    // set BK to strike temp (current method of heating strike water is in BK... may change later when BK is electric)
    ui->LE_BK_SetTemp->setText(ui->LE_Recipe_StrikeTemp->text());
    this->BK_PID_setpoint = ui->LE_Recipe_StrikeTemp->text().toDouble();
    ui->L_BK_SetTemp->setText(ui->LE_Recipe_StrikeTemp->text());
    ui->L_BK_SetTemp_Main->setText(ui->LE_Recipe_StrikeTemp->text());

    // update BK PID Settings (TODO)
}

void MainWindow::on_rb_HLT_PID_Enable_clicked()
{
    if (ui->rb_HLT_PID_Enable->isChecked())
    {
        this->HLT_PID_enable = true;
        this->update_HLT_PID_Settings();
    }
}

void MainWindow::on_rb_HLT_PID_Disable_clicked()
{
    if (ui->rb_HLT_PID_Disable->isChecked())
    {
        this->HLT_PID_enable = false;
        this->update_HLT_PID_Settings();
    }
}

void MainWindow::on_PB_BK_SetForMashIn_2_clicked()
{
    // set BK to boil temp
    ui->LE_BK_SetTemp->setText(QString::number(this->BrewConstantsPopup_Window.boiling_temp));
    this->BK_PID_setpoint = this->BrewConstantsPopup_Window.boiling_temp;
    ui->L_BK_SetTemp->setText(QString::number(this->BrewConstantsPopup_Window.boiling_temp));
    ui->L_BK_SetTemp_Main->setText(QString::number(this->BrewConstantsPopup_Window.boiling_temp));

    // update BK PID Settings (TODO)
}

void MainWindow::on_PB_BK_UpdatePID_clicked()
{
    this->BK_PID_setpoint = ui->LE_BK_SetTemp->text().toDouble();

    // update BK PID Settings (TODO)

    ui->L_BK_SetTemp->setText(ui->LE_BK_SetTemp->text());
    ui->L_BK_SetTemp_Main->setText(ui->LE_BK_SetTemp->text());
}

void MainWindow::on_PB_Clean_PumpTimer_Start_clicked()
{
    this->CleanCyclePumpTimer_StartTime = new QDateTime(QDateTime::currentDateTime());
    this->CleanCyclePumpTimer_EndTime = new QDateTime(this->CleanCyclePumpTimer_StartTime->addSecs(ui->SB_Clean_CycleTimeDuration->text().toInt()*60));
    connect(this->cleancyclepump_timer, SIGNAL(timeout()), this, SLOT(updateCleanCyclePumpTimerDisplay()));
    this->cleancyclepump_timer->start(1000);
}

void MainWindow::on_PB_Clean_PumpTimer_Stop_clicked()
{
    this->cleancyclepump_timer->stop();
    if (this->GPIO_18_val != 0 && ui->RB_Clean_PumpTimer_Auto->isChecked())
    {
        qDebug() << "stopping pump!";
        this->GPIO_18_val = 0;
        update_GPIO_Settings();
    }
}

/*void MainWindow::on_DSB_Config_MashTimerStepStartThreshold_editingFinished()
{
    this->mash_timer_step_start_threshold_temp = ui->DSB_Config_MashTimerStepStartThreshold->value();
}*/

/*void MainWindow::on_PB_Config_BoilCal_Browse_clicked()
{
    this->boil_csv_path = QFileDialog::getOpenFileName(this,
        tr("Boil Level Sensor Cal Data"), "/home/pi/", tr("CSV (*.csv)"));
    ui->LE_Config_Boil_Level_Cal_File->setText(this->boil_csv_path);
    SENSOR_THREAD.SensorWorker_Load_CSV_Data(this->boil_csv_path,this->hlt_csv_path);
}

void MainWindow::on_PB_Config_HLTCal_Browse_clicked()
{
    this->hlt_csv_path = QFileDialog::getOpenFileName(this,
        tr("HLT Level Sensor Cal Data"), "/home/pi/", tr("CSV (*.csv)"));
    ui->LE_Config_HLT_Level_Cal_File->setText(this->hlt_csv_path);
    SENSOR_THREAD.SensorWorker_Load_CSV_Data(this->boil_csv_path,this->hlt_csv_path);
}*/

void MainWindow::on_Chill_Temp_Enable_ChkBox_PitchTemp_clicked()
{
    if(ui->Chill_Temp_Enable_ChkBox_PitchTemp->isChecked())
        ui->customPlot_Chill->graph(2)->setVisible(true);
    else
        ui->customPlot_Chill->graph(2)->setVisible(false);
    ui->customPlot_Chill->replot();
}

void MainWindow::on_Boil_SetTemp_Enable_ChkBox_clicked()
{
    if(ui->Boil_SetTemp_Enable_ChkBox->isChecked())
        ui->customPlot_BK->graph(2)->setVisible(true);
    else
        ui->customPlot_BK->graph(2)->setVisible(false);
    ui->customPlot_BK->replot();
}

void MainWindow::on_MLT_Temp_Enable_ChkBox_SetTemp_clicked()
{
    if(ui->MLT_Temp_Enable_ChkBox_SetTemp->isChecked())
        ui->customPlot_MLT->graph(2)->setVisible(true);
    else
        ui->customPlot_MLT->graph(2)->setVisible(false);
    ui->customPlot_MLT->replot();
}

void MainWindow::on_HLT_SetTemp_Enable_ChkBox_clicked()
{
    if(ui->HLT_SetTemp_Enable_ChkBox->isChecked())
        ui->customPlot_HLT->graph(2)->setVisible(true);
    else
        ui->customPlot_HLT->graph(2)->setVisible(false);
    ui->customPlot_HLT->replot();
}

void MainWindow::on_PB_Settings_BrewConstants_clicked()
{
    BrewConstantsPopup_Window.setModal(true);
    BrewConstantsPopup_Window.exec();
}

void MainWindow::on_CB_HLT_InterlockEnabled_clicked()
{
    if (ui->CB_HLT_InterlockEnabled->isChecked())
        this->hlt_interlock_enabled = true;
    else
        this->hlt_interlock_enabled = false;
}

void MainWindow::on_CB_BK_InterlockEnabled_clicked()
{
    if (ui->CB_BK_InterlockEnabled->isChecked())
        this->bk_interlock_enabled = true;
    else
        this->bk_interlock_enabled = false;
}

void MainWindow::on_rb_BK_PID_Disable_clicked()
{
    if (ui->rb_BK_PID_Disable->isChecked())
    {
        this->BK_PID_enable = false;
        this->update_BK_PID_Settings();
    }
}

void MainWindow::on_rb_BK_PID_Enable_clicked()
{
    if (ui->rb_BK_PID_Enable->isChecked())
    {
        this->BK_PID_enable = true;
        this->update_BK_PID_Settings();
    }
}

void MainWindow::on_PB_Settings_DurationSettings_clicked()
{
    BrewStepDurationPopup_Window.setModal(true);
    BrewStepDurationPopup_Window.exec();
}

void MainWindow::on_PB_Settings_TempCalibrationPopup_clicked()
{
    TempProbeCalibrationPopup_Window.setModal(true);
    TempProbeCalibrationPopup_Window.exec();
}

void MainWindow::on_PB_Settings_AudioAlarmSettingsPopup_clicked()
{
    Audio_AlarmSettingsPopup_Window.setModal(true);
    Audio_AlarmSettingsPopup_Window.exec();
}

void MainWindow::on_PB_Settings_HLTPIDPopup_clicked()
{
    HLTPIDSettingsPopup_Window.setModal(true);
    HLTPIDSettingsPopup_Window.exec();
    bool value_changed = false;
    if (this->HLTPIDSettingsPopup_Window.P_val != this->HLT_PID_P_val)
    {
        this->HLT_PID_P_val = this->HLTPIDSettingsPopup_Window.P_val;
        value_changed = true;
    }
    if (this->HLTPIDSettingsPopup_Window.I_val != this->HLT_PID_I_val)
    {
        this->HLT_PID_I_val = this->HLTPIDSettingsPopup_Window.I_val;
        value_changed = true;
    }
    if (this->HLTPIDSettingsPopup_Window.D_val != this->HLT_PID_D_val)
    {
        this->HLT_PID_D_val = this->HLTPIDSettingsPopup_Window.D_val;
        value_changed = true;
    }
    if (value_changed)
        this->update_HLT_PID_Settings();
}

void MainWindow::on_PB_Settings_BKPIDPopup_clicked()
{
    BKPIDSettingsPopup_Window.setModal(true);
    BKPIDSettingsPopup_Window.exec();
    bool value_changed = false;
    if (this->BKPIDSettingsPopup_Window.P_val != this->BK_PID_P_val)
    {
        this->BK_PID_P_val = this->BKPIDSettingsPopup_Window.P_val;
        value_changed = true;
    }
    if (this->BKPIDSettingsPopup_Window.I_val != this->BK_PID_I_val)
    {
        this->BK_PID_I_val = this->BKPIDSettingsPopup_Window.I_val;
        value_changed = true;
    }
    if (this->BKPIDSettingsPopup_Window.D_val != this->BK_PID_D_val)
    {
        this->BK_PID_D_val = this->BKPIDSettingsPopup_Window.D_val;
        value_changed = true;
    }
    if (value_changed)
        this->update_BK_PID_Settings();
}

void MainWindow::on_PB_Settings_BoilCal_Browse_clicked()
{
    this->boil_csv_path = QFileDialog::getOpenFileName(this,
        tr("Boil Level Sensor Cal Data"), "/home/pi/", tr("CSV (*.csv)"));
    ui->LE_Settings_Boil_Level_Cal_File->setText(this->boil_csv_path);
    SENSOR_THREAD.SensorWorker_Load_CSV_Data(this->boil_csv_path,this->hlt_csv_path);
}

void MainWindow::on_PB_Settings_HLTCal_Browse_clicked()
{
    this->hlt_csv_path = QFileDialog::getOpenFileName(this,
        tr("HLT Level Sensor Cal Data"), "/home/pi/", tr("CSV (*.csv)"));
    ui->LE_Settings_HLT_Level_Cal_File->setText(this->hlt_csv_path);
    SENSOR_THREAD.SensorWorker_Load_CSV_Data(this->boil_csv_path,this->hlt_csv_path);
}

void MainWindow::on_DSB_HLT_AutoFill_FillToQuarts_valueChanged(double arg1)
{
    this->HLT_AutoFill_amount = ui->DSB_HLT_AutoFill_FillToQuarts->value();
}

void MainWindow::on_PB_HLT_AutoFill_Start_clicked()
{
    this->HLT_AutoFill_enabled = true;
    this->HLT_AutoFill_amount = ui->DSB_HLT_AutoFill_FillToQuarts->value();
}

void MainWindow::on_PB_HLT_AutoFill_Stop_clicked()
{
    this->HLT_AutoFill_enabled = false;
    this->GPIO_24_val = 0;
    update_GPIO_Settings();
}

void MainWindow::on_rb_HLT_FillValve_Open_clicked()
{
    this->GPIO_24_val = 1;
    update_GPIO_Settings();
}

void MainWindow::on_rb_HLT_FillValve_Closed_clicked()
{
    this->GPIO_24_val = 0;
    update_GPIO_Settings();
}

void MainWindow::on_rb_HLT_AutoStart_Enable_clicked()
{
    this->HLT_AutoStart_enabled = true;
    connect(this->clock_timer, SIGNAL(timeout()), this, SLOT(update_HLT_AutoStart()));
}

void MainWindow::on_rb_HLT_AutoStart_Disable_clicked()
{
    this->HLT_AutoStart_enabled = false;
}

void MainWindow::on_TE_HLT_AutoStart_TimeToStart_Value_timeChanged(const QTime &time)
{
    this->HLT_AutoStart_StartTime.setTime(time);
}

void MainWindow::on_TE_HLT_AutoStart_TimeOfTempAchieved_Value_timeChanged(const QTime &time)
{
    this->HLT_AutoStart_TempAchievedTime.setTime(time);
}

void MainWindow::on_rb_HLT_AutoStart_TimeToStartMode_clicked()
{
    this->HLT_AutoStart_TimeToStartMode_enabled = true;
    this->HLT_AutoStart_TimeOfTempAchievedMode_enabled = false;
    this->HLT_AutoStart_StartTime = QDateTime(ui->DTE_HLT_AutoStart_TimeToStart_Value->dateTime());
}

void MainWindow::update_HLT_AutoStart()
{
    if(this->HLT_AutoStart_enabled)
    {
        if(this->HLT_AutoStart_TimeToStartMode_enabled)
        {
            if(this->HLT_AutoStart_StartTime <= QDateTime::currentDateTime())
            {
                ui->rb_HLT_PID_Enable->setChecked(true);
                this->on_rb_HLT_PID_Enable_clicked();
            }
        }
        else if(this->HLT_AutoStart_TimeOfTempAchievedMode_enabled)
        {
            int minutes_to_TimeOfTempAchieved = QDateTime::currentDateTime().secsTo(this->HLT_AutoStart_TempAchievedTime);
            int minutes_to_tempCurrently = (ui->LE_HLT_PID_SetTemp->text().toDouble() - ui->L_HLT_CurrentTemp->text().toDouble()) / this->BrewStepDurationPopup_Window.rate_HeatStrikeWater;
            if( minutes_to_tempCurrently >= minutes_to_TimeOfTempAchieved)
            {
                ui->rb_HLT_PID_Enable->setChecked(true);
                this->on_rb_HLT_PID_Enable_clicked();
            }
        }
    }
}

void MainWindow::on_rb_HLT_AutoStart_TimeOfTempAchievedMode_clicked()
{
    this->HLT_AutoStart_TimeToStartMode_enabled = false;
    this->HLT_AutoStart_TimeOfTempAchievedMode_enabled = true;
    this->HLT_AutoStart_TempAchievedTime = QDateTime(ui->DTE_HLT_AutoStart_TimeOfTempAchieved_Value->dateTime());
}

void MainWindow::on_PB_Logs_CaptureGroundTemp_clicked()
{
    ui->L_Logs_GroundWaterTemp->setText(ui->L_HLT_CurrentTemp->text());

    qDebug() << "inserting brewday_log entry: groundwater temp: " << ui->L_Logs_GroundWaterTemp->text();

    QSqlQuery query(*this->system_db);
    query.prepare("INSERT INTO brewday_log (GroundWaterTemp)"
                  "VALUES (:groundwater_temp)");
    query.bindValue(":groundwater_temp", ui->L_Logs_GroundWaterTemp->text().toDouble());

    bool ret = query.exec();
    if (ret)
                {
                // http://www.sqlite.org/c3ref/last_insert_rowid.html
                int newID = query.lastInsertId().toInt();
                //qDebug() << "insert success for ID:" << newID;
                }
    else
    {
        qDebug() << "error inserting ground water temp into db";
        qDebug() << this->system_db->lastError();
    }
}

void MainWindow::on_LE_Logs_MashpH_editingFinished()
{
    qDebug() << "inserting brewday_log entry: mash pH: " << ui->LE_Logs_MashpH->text();

    QSqlQuery query(*this->system_db);
    query.prepare("INSERT INTO brewday_log (MashpH)"
                  "VALUES (:mash_ph)");
    query.bindValue(":mash_ph", ui->LE_Logs_MashpH->text().toDouble());

    bool ret = query.exec();
    if (ret)
                {
                // http://www.sqlite.org/c3ref/last_insert_rowid.html
                int newID = query.lastInsertId().toInt();
                //qDebug() << "insert success for ID:" << newID;
                }
    else
    {
        qDebug() << "error inserting mash ph into db";
        qDebug() << this->system_db->lastError();
    }
}

void MainWindow::on_PB_Boil_PostBoilTimer_Start_clicked()
{
    this->PostBoilTimer_StartTime = new QDateTime(QDateTime::currentDateTime());
    this->PostBoilTimer_EndTime = new QDateTime(this->PostBoilTimer_StartTime->addSecs(this->max_postboil_time*60));
    int numRows = ui->Table_PostBoil_HopTimer->rowCount();
    // set timer start values if this is the first start
    for(int i=0;i<numRows;i++)
    {
        this->hop_timer_end.push_back(this->PostBoilTimer_StartTime->addSecs((this->max_postboil_time - ui->Table_PostBoil_HopTimer->item(i,3)->text().toInt())*60));
    }
    connect(this->postboil_timer, SIGNAL(timeout()), this, SLOT(updatePostBoilTimerDisplay()));

    this->postboil_timer->start(1000);
}

void MainWindow::on_PB_Boil_PostBoilTimer_Stop_clicked()
{

}

void MainWindow::on_DTE_HLT_AutoStart_TimeToStart_Value_editingFinished()
{
    this->HLT_AutoStart_StartTime = QDateTime(ui->DTE_HLT_AutoStart_TimeToStart_Value->dateTime());
}

void MainWindow::on_DTE_HLT_AutoStart_TimeOfTempAchieved_Value_editingFinished()
{
    this->HLT_AutoStart_TempAchievedTime = QDateTime(ui->DTE_HLT_AutoStart_TimeOfTempAchieved_Value->dateTime());
}

void MainWindow::on_PB_Recipe_DeleteHop_clicked()
{
    if(ui->Table_Recipe_HopAdditions->rowCount() > 0)
    {

        if(ui->Table_Recipe_HopAdditions->item(ui->Table_Recipe_HopAdditions->rowCount()-1,4)->text() == "Boil")
            ui->Table_Boil_HopTimer->removeRow(ui->Table_Boil_HopTimer->rowCount()-1);
        else if (ui->Table_Recipe_HopAdditions->item(ui->Table_Recipe_HopAdditions->rowCount()-1,4)->text() == "Aroma")
            ui->Table_PostBoil_HopTimer->removeRow(ui->Table_PostBoil_HopTimer->rowCount()-1);

        ui->Table_Recipe_HopAdditions->removeRow(ui->Table_Recipe_HopAdditions->rowCount()-1);
    }
}

void MainWindow::on_PB_Recipe_DeleteStep_clicked()
{
    if(ui->Table_Recipe_MashSteps->rowCount() > 0)
    {
        ui->Table_Mash_StepTimer->removeRow(ui->Table_Mash_StepTimer->rowCount()-1);
        ui->Table_Recipe_MashSteps->removeRow(ui->Table_Recipe_MashSteps->rowCount()-1);
    }
}

void MainWindow::on_PB_Recipe_DeleteFermentable_clicked()
{
    if(ui->table_Recipe_Fermentables->rowCount() > 0)
        ui->table_Recipe_Fermentables->removeRow(ui->table_Recipe_Fermentables->rowCount()-1);
}
