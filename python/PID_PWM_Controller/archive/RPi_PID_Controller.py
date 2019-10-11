from PID import PID
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
SYSTEM_DB="/DB/brewing_db.db" #location of sqlite db file
base_dir = '/sys/bus/w1/devices/' #location of 1wire sensor data
#HLT_DIR = /sys/bus/w1/devices/28-HLT_TEST
#Boil_DIR =/sys/bus/w1/devices/28-Boil_TEST
#Mash_DIR =/sys/bus/w1/devices/28-Mash_TEST
HLT_DIR ="/tmp/sys/bus/w1/devices/28-HLT_TEST"
Boil_DIR ="/tmp/sys/bus/w1/devices/28-Boil_TEST"
Mash_DIR ="/tmp/sys/bus/w1/devices/28-Mash_TEST"


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
        lines = read_temp_raw()
    equals_pos = lines[1].find('t=')
    if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:]
        temp_c = float(temp_string) / 1000.0
        temp_f = temp_c * 9.0 / 5.0 + 32.0
        #return temp_c, temp_f
        return temp_f

# setup database
try:
    con = lite.connect(SYSTEM_DB)
    cur = con.cursor()  
    cur.executescript("""
        DROP TABLE IF EXISTS sensor_data;
	DROP TABLE IF EXISTS HLT_pid_settings;
        DROP TABLE IF EXISTS Boil_pid_settings;
        DROP TABLE IF EXISTS Mash_pid_settings;
        CREATE TABLE sensor_data(Id INTEGER PRIMARY KEY AUTOINCREMENT, date TEXT,HLT_current_temp FLOAT,Boil_current_temp,Mash_current_temp,HLT_element_status INT,Boil_element_status INTEGER);
        CREATE TABLE HLT_pid_settings(Id INTEGER PRIMARY KEY AUTOINCREMENT, date TEXT,PID_set_temp FLOAT,PID_P_val FLOAT,PID_I_val FLOAT,PID_D_val FLOAT,ENABLE_bool INTEGER);
        CREATE TABLE Boil_pid_settings(Id INTEGER PRIMARY KEY AUTOINCREMENT, date TEXT,PID_set_temp FLOAT,PID_P_val FLOAT,PID_I_val FLOAT,PID_D_val FLOAT,ENABLE_bool INTEGER);
        CREATE TABLE Mash_pid_settings(Id INTEGER PRIMARY KEY AUTOINCREMENT, date TEXT,PID_set_temp FLOAT,PID_P_val FLOAT,PID_I_val FLOAT,PID_D_val FLOAT,ENABLE_bool INTEGER);
        INSERT INTO HLT_pid_settings(PID_set_temp, PID_P_val,PID_I_val, PID_D_val, ENABLE_bool) VALUES (157.2,1.0, 2.0, 3.0, 1);
        """)
    con.commit()
    
except lite.Error, e:
    if con:
        con.rollback()
    print "Error %s:" % e.args[0]
    sys.exit(1)
 
#finally:
#    if con:
#        con.close() 
print "Database successfully created"

# setup RPi GPIO as PWM outputs
GPIO.setmode(GPIO.BCM)
GPIO.setup(23,GPIO.OUT) #for HLT PWM
#GPIO.setup(24,GPIO.OUT) #for Boil PWM
#GPIO.setup(25,GPIO.OUT) #for Mash PWM
HLT_PWM = GPIO.PWM(23,50) #PWM instance for HLT (0 duty cycle to start)
HLT_PWM.ChangeFrequency(1.0) #set PWM freq to 1Hz (plenty fast enough for a heating element)
#Boil_PWM = GPIO.PWM(24,0) #PWM instance for Boil (0 duty cycle to start)
#Boil_PWM.ChangeFrequency(1.0) #set PWM freq to 1Hz (plenty fast enough for a heating element)
#Mash_PWM = GPIO.PWM(24,0) #PWM instance for Mash (0 duty cycle to start)
#Mash_PWM.ChangeFrequency(1.0) #set PWM freq to 1Hz (plenty fast enough for a heating element)

# create PID for HLT temp control
HLT_PID = PID()
# create PID for Boil temp control
#Boil_PID = PID()
# create PID for Mash temp control (recirc)
#Mash_PID = PID()

#get PID setpoint, P, I, D vals from database
    #HLT_PID.setPoint(setpoint from latest entry of db)
    #HLT_PID.setKp(P val from db)
    #HLT_PID.setKi(I val from db)
    #HLT_PID.setKd(D val from db)
last_row = cur.lastrowid
cur.execute("SELECT * FROM HLT_pid_settings WHERE Id=(SELECT MAX(Id) FROM HLT_pid_settings)")
row = cur.fetchone()
print row[2], row[3], row[4], row[5]
HLT_PID.setPoint(row[2])
HLT_PID.setKp(row[3])
HLT_PID.setKi(row[4])
HLT_PID.setKd(row[5])
   

while 1:
    print "PID loop"
    print "PID settings: %f" % HLT_PID.getPoint()
    # get current HLT/Boil/Mash temps from latest sensor_data table entry
    HLT_cur_temp = read_temp(HLT_DIR)
    Boil_cur_temp = read_temp(Boil_DIR)
    Mash_cur_temp = read_temp(Mash_DIR)

    print HLT_cur_temp, Boil_cur_temp, Mash_cur_temp

    #cur.execute("SELECT * FROM sensor_data WHERE Id=(SELECT MAX(Id) FROM sensor_data)")
    #row = cur.fetchone()
    # call new_pwm_val = PID.update(current temp) for HLT/Boil/Mash PID

    #HLT_PWM.ChangeDutyCycle(new_HLT_pwm_val)
    #Boil_PWM.ChangeDutyCycle(new_Boil_pwm_val)
    #Mash_PWM.ChangeDutyCycle(new_Mash_pwm_val)

    # check if any new PID settings have been made, if so update PID
        #check if latest entry in pid_settings is newer than the last one
        # if new, do the following:
        #HLT_PID.setKp(new_HLT_p_val)
        #HLT_PID.setKi(new_HLT_i_val)
        #HLT_PID.setKd(new_HLT_d_val)
        #Boil_PID.setKp(new_Boil_p_val)
        #Boil_PID.setKi(new_Boil_i_val)
        #Boil_PID.setKd(new_Boil_d_val)
        #Mash_PID.setKp(new_Mash_p_val)
        #Mash_PID.setKi(new_Mash_i_val)
        #Mash_PID.setKd(new_Mash_d_val)

    # sleep and wait to make next measurement/PID adjustment
    time.sleep(1)
