#ifndef MYBREW_H
#define MYBREW_H

#include <QMainWindow>
#include "SensorWorkerThread.h"
#include "PIDThread.h"
#include <QtSql>
#include "qcustomplot.h"
#include "beerxmlrecipe.h"
#include "brewconstantspopup.h"
#include "brewstepdurationpopup.h"
#include "tempprobecalibrationpopup.h"
#include "bkpidsettingspopup.h"
#include "hltpidsettingspopup.h"
#include "audio_alarmsettingspopup.h"
#include <list>
#include "myBrewLibGlobal.h"


namespace Ui {
class myBrew;
}

class WIDGETSLIBSHARED_EXPORT myBrew : public QMainWindow
{
    Q_OBJECT
    //MAKE_TESTABLE(myBrew);
    friend class myBrewTests;

public:
    explicit myBrew(QWidget *parent = 0);
    ~myBrew();

private slots:
    void on_actionQuit_triggered();
    void updateClockDisplay();
    void updateElapsedDisplay();
    void updateBoilTimerDisplay();
    void updatePostBoilTimerDisplay();
    void updateMashTimerDisplay();
    void updateVorlaufTimerDisplay();
    void updateCleanCyclePumpTimerDisplay();
    void updateTimer1Display();
    void updateTimer2Display();
    void updateTimer3Display();
    void updateTimer4Display();
    void on_PB_Checklist_Start_clicked();
    void on_CB_Checklist_FillHLT_clicked();
    void on_CB_Checklist_HeatStrikeWater_clicked();
    void on_CB_Checklist_TransferStrikeWater_clicked();
    void on_CB_Checklist_DoughIn_clicked();
    void on_CB_Checklist_Mash_clicked();
    void on_CB_Checklist_MashOut_clicked();
    void on_CB_Checklist_Vorlauf1_clicked();
    void on_CB_Checklist_BatchSparge1_clicked();
    void on_CB_Checklist_Vorlauf2_clicked();
    void on_CB_Checklist_BatchSparge2_clicked();
    void on_CB_Checklist_Boil_clicked();
    void on_CB_Checklist_Whirlpool_clicked();
    void on_CB_Checklist_Chill_clicked();
    void on_CB_Checklist_Clean_clicked();
    void on_PB_Checklist_Reset_clicked();
    void on_LE_Recipe_BoilTime_editingFinished();
    void on_PB_Boil_BoilTimer_Start_clicked();
    void on_PB_Boil_BoilTimer_Stop_clicked();
    void on_PB_Recipe_AddHop_clicked();
    void on_PB_MLT_MashTimer_Start_clicked();
    void on_PB_MLT_VorlaufTimer_Start_clicked();
    void on_PB_Recipe_AddStep_clicked();
    void on_PB_MLT_MashTimer_Stop_clicked();
    void on_PB_MLT_VorlaufTimer_Stop_clicked();
    void SensorHandler(double hlt_temp, double hlt_pwm, double mlt_temp, double boil_temp, double chill_temp, double boil_level, double hlt_level, bool HLT_VALID, bool MASH_VALID, bool BOIL_VALID, bool CHILL_VALID, bool BOIL_LEVEL_VALID, bool HLT_LEVEL_VALID);
    void HLT_PIDHandler(double setpoint, double P, double I, double D);
    void BK_PIDHandler(double setpoint, double P, double I, double D);
    void createChecklist_DB();
    void insertChecklist_DB_Entry(QString timestamp_local, int timestamp_epoch, QString Checklist_Entry);
    void update_GPIO_Settings();
    void update_HLT_PID_Settings();
    void update_BK_PID_Settings();
    void update_HLT_AutoStart();
    //void on_DSB_Config_VorlaufTime_editingFinished();

    void on_PB_HLT_UpdatePID_clicked();

    //void on_PB_HLT_CaptureGroundTemp_clicked();

    void on_PB_Timer1_Set_clicked();

    void on_PB_Timer1_Start_clicked();

    void on_PB_Timer1_Stop_clicked();

    void on_PB_Timer2_Set_clicked();

    void on_PB_Timer2_Start_clicked();

    void on_PB_Timer2_Stop_clicked();

    void on_PB_Timer3_Set_clicked();

    void on_PB_Timer3_Start_clicked();

    void on_PB_Timer3_Stop_clicked();

    void on_PB_Timer4_Set_clicked();

    void on_PB_Timer4_Start_clicked();

    void on_PB_Timer4_Stop_clicked();

    //void on_DSB_Config_TempDropHLTtoHERMS_editingFinished();

    void on_CB_HLT_TimeRangeScale_currentIndexChanged(int index);

    void on_CB_MLT_TimeRangeScale_currentIndexChanged(int index);

    void on_CB_BK_TimeRangeScale_currentIndexChanged(int index);

    void on_CB_Chill_TimeRangeScale_currentIndexChanged(int index);

    void on_PB_MLT_UpdateHLTPIDSetTemp_clicked();

    void on_r_StirMotor_ON_clicked();

    void on_r_StirMotor_OFF_clicked();

    void on_LE_Recipe_PitchTemp_editingFinished();

    //void on_Table_Config_TempProbeCalibration_cellChanged(int row, int column);

    void on_HLT_Auto_Scale_Enable_ChkBox_clicked();

    void on_CB_Logs_TimeRangeScale_currentIndexChanged(int index);

    void on_Logs_Auto_Scale_Enable_ChkBox_clicked();

    void on_MLT_Auto_Scale_Enable_ChkBox_clicked();

    void on_Boil_Auto_Scale_Enable_ChkBox_clicked();

    void on_Chill_Auto_Scale_Enable_ChkBox_clicked();

    void on_PB_Logs_Browse_SaveLogAs_clicked();

    void on_PB_Logs_Save_SaveLogAs_clicked();

    void export_db_table(QString table_name);

    void on_HLT_Temp_Enable_ChkBox_HLT_clicked();

    void on_HLT_PWM_Enable_ChkBox_clicked();

    void on_MLT_Temp_Enable_ChkBox_HERMS_clicked();

    void on_MLT_Temp_Enable_ChkBox_HLT_clicked();

    void on_Boil_Temp_Enable_ChkBox_Boil_clicked();

    void on_Boil_PWM_Enable_ChkBox_clicked();

    void on_Chill_Temp_Enable_ChkBox_Chill_clicked();

    void on_Chill_Temp_Enable_ChkBox_Boil_clicked();

    void on_Logs_Temp_Enable_ChkBox_HLT_clicked();

    void on_Logs_PWM_Enable_ChkBox_clicked();

    void on_Logs_Temp_Enable_ChkBox_HERMS_clicked();

    void on_Logs_Temp_Enable_ChkBox_Boil_clicked();

    void on_Logs_PWM_Enable_ChkBox_2_clicked();

    void on_Logs_Temp_Enable_ChkBox_Chill_clicked();
    void soundAlarm(QString alarmText);

    //void on_PB_Config_AlarmBrowse_clicked();

    void on_PB_AlarmText_clicked();

    void on_PB_Recipe_ImportBeerXMLRecipe_Browse_clicked();
    double CtoF(double celsius);
    double LtoGal(double liters);
    double KGtoLB(double kilograms);

    void on_PB_Recipe_AddFermentable_clicked();

    void on_LE_Logs_PreBoilGravity_editingFinished();

    void on_PB_BK_SetForMashIn_clicked();

    void on_rb_HLT_PID_Enable_clicked();

    void on_rb_HLT_PID_Disable_clicked();

    void on_PB_BK_SetForMashIn_2_clicked();
    QTime calculate_HeatTime(double setpoint, double current_temp, double heating_rate_per_min);

    void on_PB_BK_UpdatePID_clicked();

    void on_PB_Clean_PumpTimer_Start_clicked();

    void on_PB_Clean_PumpTimer_Stop_clicked();

    void clear_and_disable_label(QLabel* label, QString new_label_text, bool enable_disable);

    void clear_and_disable_checkbox(QCheckBox* checkbox, bool checked, bool enable_disable);

    //void on_DSB_Config_MashTimerStepStartThreshold_editingFinished();

    //void on_PB_Config_BoilCal_Browse_clicked();

    //void on_PB_Config_HLTCal_Browse_clicked();

    void on_Chill_Temp_Enable_ChkBox_PitchTemp_clicked();

    void on_Boil_SetTemp_Enable_ChkBox_clicked();

    void on_MLT_Temp_Enable_ChkBox_SetTemp_clicked();

    void on_HLT_SetTemp_Enable_ChkBox_clicked();

    void on_PB_Settings_BrewConstants_clicked();

    void on_CB_HLT_InterlockEnabled_clicked();

    void on_CB_BK_InterlockEnabled_clicked();

    void on_rb_BK_PID_Disable_clicked();

    void on_rb_BK_PID_Enable_clicked();

    void on_PB_Settings_DurationSettings_clicked();

    void on_PB_Settings_TempCalibrationPopup_clicked();


    void on_PB_Settings_AudioAlarmSettingsPopup_clicked();

    void on_PB_Settings_HLTPIDPopup_clicked();

    void on_PB_Settings_BKPIDPopup_clicked();

    void on_PB_Settings_BoilCal_Browse_clicked();

    void on_PB_Settings_HLTCal_Browse_clicked();

    void on_DSB_HLT_AutoFill_FillToQuarts_valueChanged(double arg1);

    void on_PB_HLT_AutoFill_Start_clicked();

    void on_PB_HLT_AutoFill_Stop_clicked();

    void on_rb_HLT_FillValve_Open_clicked();

    void on_rb_HLT_FillValve_Closed_clicked();

    void on_rb_HLT_AutoStart_Enable_clicked();

    void on_rb_HLT_AutoStart_Disable_clicked();

    void on_TE_HLT_AutoStart_TimeToStart_Value_timeChanged(const QTime &time);

    void on_TE_HLT_AutoStart_TimeOfTempAchieved_Value_timeChanged(const QTime &time);

    void on_rb_HLT_AutoStart_TimeToStartMode_clicked();

    void on_rb_HLT_AutoStart_TimeOfTempAchievedMode_clicked();

    void on_PB_Logs_CaptureGroundTemp_clicked();

    void on_LE_Logs_MashpH_editingFinished();

    void on_PB_Boil_PostBoilTimer_Start_clicked();

    void on_PB_Boil_PostBoilTimer_Stop_clicked();

    void on_DTE_HLT_AutoStart_TimeToStart_Value_editingFinished();

    void on_DTE_HLT_AutoStart_TimeOfTempAchieved_Value_editingFinished();

    void on_PB_Recipe_DeleteHop_clicked();

    void on_PB_Recipe_DeleteStep_clicked();

    void on_PB_Recipe_DeleteFermentable_clicked();

private:
    Ui::myBrew *ui;
    QTimer *clock_timer;
    QTimer *elapsed_timer;
    QTimer *boil_timer;
    QTimer *postboil_timer;
    QTimer *mash_timer;
    QTimer *vorlauf_timer;
    QTimer *cleancyclepump_timer;
    QTimer *timer1_timer;
    QTimer *timer2_timer;
    QTimer *timer3_timer;
    QTimer *timer4_timer;
    QDateTime *Checklist_StartTime;
    QDateTime *BoilTimer_StartTime;
    QDateTime *BoilTimer_EndTime;
    QDateTime *BoilTimer_StopTime;
    QDateTime *PostBoilTimer_StartTime;
    QDateTime *PostBoilTimer_EndTime;
    QDateTime *PostBoilTimer_StopTime;
    QDateTime *MashTimer_StartTime;
    QDateTime *MashTimer_EndTime;
    QDateTime *MashStepTimer_StartTime;
    QDateTime *MashStepTimer_EndTime;
    QDateTime *VorlaufTimer_StartTime;
    QDateTime *VorlaufTimer_EndTime;
    QDateTime *CleanCyclePumpTimer_StartTime;
    QDateTime *CleanCyclePumpTimer_EndTime;
    QDateTime *Timer1_StartTime;
    QDateTime *Timer1_EndTime;
    QDateTime *Timer2_StartTime;
    QDateTime *Timer2_EndTime;
    QDateTime *Timer3_StartTime;
    QDateTime *Timer3_EndTime;
    QDateTime *Timer4_StartTime;
    QDateTime *Timer4_EndTime;
    QDateTime *ETC_Time;
    void calculate_ETC();
    SensorWorkerThread SENSOR_THREAD;
    PIDThread HLT_PID_Thread;
    PIDThread Boil_PID_Thread;
    QSqlDatabase *system_db;
    double HLT_PID_setpoint;
    double HLT_PID_P_val;
    double HLT_PID_I_val;
    double HLT_PID_D_val;
    bool HLT_PID_enable;
    double BK_PID_setpoint;
    double BK_PID_P_val;
    double BK_PID_I_val;
    double BK_PID_D_val;
    bool BK_PID_enable;
    int GPIO_18_val; // pump relay
    int GPIO_22_val; // stir motor relay
    int GPIO_24_val; // hlt fill valve relay
    int GPIO_27_val;
    double tempDropFromHLTtoHERMS;
    int current_mash_step;
    int hlt_range_ticks;
    int mlt_range_ticks;
    int bk_range_ticks;
    int chill_range_ticks;
    int logs_range_ticks;
    double HLT_Temp_Correction;
    double HERMS_Temp_Correction;
    double BK_Temp_Correction;
    double Chill_Temp_Correction;
    QString CSV_fileName;
    QString Alarm_SoundFile;
    QString BeerXML_Recipe_filename;
    BeerXMLRecipe *recipe;
    double totalRiseTime;
    double mash_timer_step_start_threshold_temp;
    QString boil_csv_path;
    QString hlt_csv_path;
    list <double> boil_sensor_temp_history_time;
    list <double> boil_sensor_temp_history_value;
    list <double> hlt_sensor_temp_history_time;
    list <double> hlt_sensor_temp_history_value;
    vector <QDateTime> hop_timer_end;
    bool boil_started;
    BrewConstantsPopup BrewConstantsPopup_Window;
    bool hlt_interlock_enabled;
    bool bk_interlock_enabled;
    BrewStepDurationPopup BrewStepDurationPopup_Window;
    TempProbeCalibrationPopup TempProbeCalibrationPopup_Window;
    Audio_AlarmSettingsPopup Audio_AlarmSettingsPopup_Window;
    BKPIDSettingsPopup BKPIDSettingsPopup_Window;
    HLTPIDSettingsPopup HLTPIDSettingsPopup_Window;
    bool HLT_AutoFill_enabled;
    double HLT_AutoFill_amount;
    double HLT_FillValve_shutoff_time_amount;
    int max_postboil_time;
    bool HLT_AutoStart_enabled;
    QDateTime HLT_AutoStart_StartTime;
    QDateTime HLT_AutoStart_TempAchievedTime;
    bool HLT_AutoStart_TimeToStartMode_enabled;
    bool HLT_AutoStart_TimeOfTempAchievedMode_enabled;
};

#endif
