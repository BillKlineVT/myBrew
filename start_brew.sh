#!/bin/bash

echo "starting scripts"

xterm -e "sudo python /home/pi/BrewControl_PiScripts_04132014/PID_PWM_Controller/RPi_PID_HLT.py"& 
sleep 2
xterm -e "sudo python /home/pi/BrewControl_PiScripts_04132014/Sensor_Poll/temp_sensor_poll.py"&  >/tmp/temp_sensor_poll.log 2>/tmp/temp_sensor_poll.log
sleep 2
xterm -e "sudo python /home/pi/BrewControl_PiScripts_04132014/GPIO_DB_Listener/RPi_GPIO_DB_Listener.py"& >/tmp/RPi_GPIO_DB_Listener.log 2>/tmp/RPi_GPIO_DB_Listener.log
sleep 5

xterm -e "sudo /home/pi/ElectricBrewingClone/ElectricBrewingClone"& >/tmp/ElectricBrewingClone.log 2>/tmp/ElectricBrewingClone.log
