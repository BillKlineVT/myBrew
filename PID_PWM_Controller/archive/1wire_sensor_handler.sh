#!/bin/bash

# add drivers at startup
sudo modprobe w1-gpio
sudo modprobe w1-therm

# go to directory of sensor
# change this to match sensor ID's for HLT/Boil/Mash
HLT_DIR=/sys/bus/w1/devices/28-HLT_TEST
Boil_DIR=/sys/bus/w1/devices/28-Boil_TEST
Mash_DIR=/sys/bus/w1/devices/28-Mash_TEST

function getTempF(sensor_dir)

