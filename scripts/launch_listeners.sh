#!/bin/bash
sudo python GPIO_DB_Listener/RPi_GPIO_DB_Listener.py &
sleep 1
sudo python PID_PWM_Controller/RPi_PID_HLT.py &
sleep 1
sudo python Sensor_Poll/temp_sensor_poll.py &

