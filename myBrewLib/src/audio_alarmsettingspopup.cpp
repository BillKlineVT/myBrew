#include "audio_alarmsettingspopup.h"
#include "ui_audio_alarmsettingspopup.h"

Audio_AlarmSettingsPopup::Audio_AlarmSettingsPopup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Audio_AlarmSettingsPopup)
{
    ui->setupUi(this);
}

Audio_AlarmSettingsPopup::~Audio_AlarmSettingsPopup()
{
    delete ui;
}

void Audio_AlarmSettingsPopup::on_PB_AudioAlarmSetting_Save_clicked()
{
    this->alarm_sound_file_path = ui->LE_Config_AlarmSoundFile->text();
}

void Audio_AlarmSettingsPopup::on_PB_AudioAlarmSetting_Close_clicked()
{
    this->close();
}

void Audio_AlarmSettingsPopup::on_PB_AlarmBrowse_clicked()
{
    this->alarm_sound_file_path = QFileDialog::getOpenFileName(this,
        tr("Alarm Sound File"), "/home/pi/"+this->alarm_sound_file_path, tr("All Files (*.*)"));
    ui->LE_Config_AlarmSoundFile->setText(this->alarm_sound_file_path);
}
