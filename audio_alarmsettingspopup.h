#ifndef AUDIO_ALARMSETTINGSPOPUP_H
#define AUDIO_ALARMSETTINGSPOPUP_H

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class Audio_AlarmSettingsPopup;
}

class Audio_AlarmSettingsPopup : public QDialog
{
    Q_OBJECT

public:
    explicit Audio_AlarmSettingsPopup(QWidget *parent = 0);
    ~Audio_AlarmSettingsPopup();
    QString alarm_sound_file_path;

public slots:
    void on_PB_AudioAlarmSetting_Save_clicked();
private slots:
    void on_PB_AudioAlarmSetting_Close_clicked();

    void on_PB_AlarmBrowse_clicked();

private:
    Ui::Audio_AlarmSettingsPopup *ui;
};

#endif // AUDIO_ALARMSETTINGSPOPUP_H
