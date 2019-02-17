import time
import datetime
import os
import sys
import glob
import sqlite3 as lite
from time import sleep
import RPi.GPIO as GPIO

#globals
con="" #db connection
SYSTEM_DB="/DB/brewing_db1.db" #location of sqlite db file
base_dir = '/sys/bus/w1/devices/' #location of 1wire sensor data
#HLT_DIR = "/sys/bus/w1/devices/28-HLT_TEST"
#HLT_DIR ="/tmp/sys/bus/w1/devices/28-HLT_TEST"
HLT_DIR = "/sys/devices/w1_bus_master1/28-000004cd65c3"

# set GPIO mode
GPIO.setmode(GPIO.BCM)
GPIO.setup(2,GPIO.OUT)
GPIO.setup(3,GPIO.OUT)
GPIO.setup(4,GPIO.OUT)
GPIO.setup(7,GPIO.OUT)
GPIO.setup(8,GPIO.OUT)
GPIO.setup(9,GPIO.OUT)
GPIO.setup(10,GPIO.OUT)
GPIO.setup(11,GPIO.OUT)
GPIO.setup(14,GPIO.OUT)
GPIO.setup(15,GPIO.OUT)
GPIO.setup(17,GPIO.OUT)
GPIO.setup(18,GPIO.OUT)
GPIO.setup(22,GPIO.OUT)
GPIO.setup(23,GPIO.OUT)
GPIO.setup(24,GPIO.OUT)
GPIO.setup(25,GPIO.OUT)

#setup 1wire sensor interface
os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')

def read_temp_raw(sensor_dir):
    device_file = sensor_dir + '/w1_slave'
    f = open(device_file, 'r')
    lines = f.readlines()
    f.close()
    return lines
 
def read_temp(sensor_dir):
    lines = read_temp_raw(sensor_dir)
    while lines[0].strip()[-3:] != 'YES':
        time.sleep(0.2)
        lines = read_temp_raw(sensor_dir)
    equals_pos = lines[1].find('t=')
    if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:]
        temp_c = float(temp_string) / 1000.0
        temp_f = temp_c * 9.0 / 5.0 + 32.0
        #return temp_c, temp_f
        return temp_f

GPIO_2_State=False
GPIO_3_State=False
GPIO_4_State=False
GPIO_7_State=False
GPIO_8_State=False
GPIO_9_State=False
GPIO_10_State=False
GPIO_11_State=False
GPIO_14_State=False
GPIO_15_State=False
GPIO_17_State=False
GPIO_18_State=False
GPIO_22_State=False
GPIO_23_State=False
GPIO_24_State=False
GPIO_25_State=False


print "GPIO Test Program"

while True:
    menu_choice = input("Menu Options:\n1. GPIO 2\n2. GPIO 3\n3. GPIO 4\n4. GPIO 7\n5. GPIO 8\n6. GPIO 9\n7. GPIO 10\n8. GPIO 11\n9. GPIO 14\n10. GPIO 15\n11. GPIO 17\n12. GPIO 18\n13. GPIO 22\n14. GPIO 23\n15. GPIO 24\n16. GPIO 25\n17. Exit\n")
    if menu_choice == 1:
        submenu_choice = input("1. GPIO 2: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_2_State = not GPIO_2_State
            GPIO.output(2,GPIO_2_State)
            print "toggling GPIO to " + str(GPIO_2_State)
    elif menu_choice == 2:
        submenu_choice = input("1. GPIO 3: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_3_State = not GPIO_3_State
            GPIO.output(3,GPIO_3_State)
            print "toggling GPIO to " + str(GPIO_3_State)
    elif menu_choice == 3:
        submenu_choice = input("1. GPIO 4: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_4_State = not GPIO_4_State
            GPIO.output(4,GPIO_4_State)
            print "toggling GPIO to " + str(GPIO_4_State)
    elif menu_choice == 4:
        submenu_choice = input("1. GPIO 7: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_7_State = not GPIO_7_State
            GPIO.output(7,GPIO_7_State)
            print "toggling GPIO to " + str(GPIO_7_State)
    elif menu_choice == 5:
        submenu_choice = input("1. GPIO 8: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_8_State = not GPIO_8_State
            GPIO.output(8,GPIO_8_State)
            print "toggling GPIO to " + str(GPIO_8_State)
    elif menu_choice == 6:
        submenu_choice = input("1. GPIO 9: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_9_State = not GPIO_9_State
            GPIO.output(9,GPIO_9_State)
            print "toggling GPIO to " + str(GPIO_9_State)
    elif menu_choice == 7:
        submenu_choice = input("1. GPIO 10: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_10_State = not GPIO_10_State
            GPIO.output(10,GPIO_10_State)
            print "toggling GPIO to " + str(GPIO_10_State)
    elif menu_choice == 8:
        submenu_choice = input("1. GPIO 11: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_11_State = not GPIO_11_State
            GPIO.output(11,GPIO_11_State)
            print "toggling GPIO to " + str(GPIO_11_State)
    elif menu_choice == 9:
        submenu_choice = input("1. GPIO 14: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_14_State = not GPIO_14_State
            GPIO.output(14,GPIO_14_State)
            print "toggling GPIO to " + str(GPIO_14_State)
    elif menu_choice == 10:
        submenu_choice = input("1. GPIO 15: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_15_State = not GPIO_15_State
            GPIO.output(15,GPIO_15_State)
            print "toggling GPIO to " + str(GPIO_15_State)
    elif menu_choice == 11:
        submenu_choice = input("1. GPIO 17: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_17_State = not GPIO_17_State
            GPIO.output(17,GPIO_17_State)
            print "toggling GPIO to " + str(GPIO_17_State)
    elif menu_choice == 12:
        submenu_choice = input("1. GPIO 18: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_18_State = not GPIO_18_State
            GPIO.output(18,GPIO_18_State)
            print "toggling GPIO to " + str(GPIO_18_State)
    elif menu_choice == 13:
        submenu_choice = input("1. GPIO 22: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_22_State = not GPIO_22_State
            GPIO.output(22,GPIO_22_State)
            print "toggling GPIO to " + str(GPIO_22_State)
    elif menu_choice == 14:
        submenu_choice = input("1. GPIO 23: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_23_State = not GPIO_23_State
            GPIO.output(23,GPIO_23_State)
            print "toggling GPIO to " + str(GPIO_23_State)
    elif menu_choice == 15:
        submenu_choice = input("1. GPIO 24: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_24_State = not GPIO_24_State
            GPIO.output(24,GPIO_24_State)
            print "toggling GPIO to " + str(GPIO_24_State)
    elif menu_choice == 16:
        submenu_choice = input("1. GPIO 25: Toggle ON/OFF\n")
        if submenu_choice == 1:
            GPIO_25_State = not GPIO_25_State
            GPIO.output(25,GPIO_25_State)
            print "toggling GPIO to " + str(GPIO_25_State)
    elif menu_choice == 17:
        print "Exiting..."
        sys.exit()
    else:
        print "not a valid menu option"

# setup RPi GPIO as PWM outputs
#GPIO.setmode(GPIO.BCM)
#GPIO.setup(23,GPIO.OUT) #for HLT PWM
#HLT_PWM = GPIO.PWM(23,1) #PWM instance for HLT @ 1Hz
#HLT_PWM.start(0)

