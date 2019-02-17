#!/bin/bash

echo "starting scripts"

sudo python /home/pi/BrewControl_PiScripts_04132014/PID_PWM_Controller/RPi_PID_HLT.py& 
sleep 2
sudo python /home/pi/BrewControl_PiScripts_04132014/Sensor_Poll/temp_sensor_poll.py
sleep 2
sudo python /home/pi/BrewControl_PiScripts_04132014/GPIO_DB_Listener/RPi_GPIO_DB_Listener.py
sleep 5

#xterm -e "sudo /home/pi/ElectricBrewingClone-build-embedded-Qt_4_8_2_in_PATH__System__Release/ElectricBrewingClone"& >/tmp/ElectricBrewingClone.log 2>/tmp/ElectricBrewingClone.log
