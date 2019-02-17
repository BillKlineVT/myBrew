#ifndef BREWERYSETTINGS_H
#define BREWERYSETTINGS_H
#include <QString>

class BrewerySettings
{
public:
    BrewerySettings();
    double TempProbeCalibration_HLT;
    double TempProbeCalibration_HERMSOut;
    double TempProbeCalibration_BK;
    double TempProbeCalibration_ChillerOut;
    double BrewingStepDuration_FillHLT;
    double BrewingStepDuration_HeatStrikeWater;
    double BrewingStepDuration_TransferStrike;
    double BrewingStepDuration_DoughIn;
    double BrewingStepDuration_MashOut;
    double BrewingStepDuration_BatchSparge;
    double BrewingStepDuration_Vorlauf;
    double BrewingStepDuration_HeatWortToBoil;
    double BrewingStepDuration_Whirlpool;
    double BrewingStepDuration_Chill;
    double BrewingStepDuration_Clean;
    double Constants_VorlaufTime;
    double Constants_TempDropFromHLTToHERMSOut;
    double Constants_TempDropFromHERMSOutToMLTIn;
    double Constants_HLTHeatingElementInterlockVolume;
    double Constants_BKHeatingElementInterlockVolume;
    double Constants_GroundWaterTemp;
    double Constants_BoilingTemp;
    double Constants_MashTimerStepStartThreshold;
    double Constants_MashThickness;
    double Constants_WortCoolingShrinkage;
    double Constants_BoilOffRate;
    double Constants_BrewKettleTrubLoss;
    double Constants_MashTunLoss;
    double Constants_GrainAbsorption;
    double Constants_SpargeGrainAbsorption;
    double Constants_FermenterLoss;
    double Constants_HLTFillValveShutoffTimeVolume;
    QString AudioAlarm_AlarmSoundFilePath;
    double HLTPID_PVal;
    double HLTPID_IVal;
    double HLTPID_DVal;
    double BKPID_PVal;
    double BKPID_IVal;
    double BKPID_DVal;
    QString BKLevelSensor_CalibrationFilePath;
    QString HLTLevelSensor_CalibrationFilePath;
};

#endif // BREWERYSETTINGS_H
