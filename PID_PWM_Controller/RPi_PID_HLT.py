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
SYSTEM_DB="/DB/brewing_db1.db" #location of sqlite db file
base_dir = '/sys/bus/w1/devices/' #location of 1wire sensor data
#HLT_DIR = "/sys/bus/w1/devices/28-HLT_TEST"
#HLT_DIR ="/tmp/sys/bus/w1/devices/28-HLT_TEST"
HLT_DIR = "/sys/devices/w1_bus_master1/28-000004cd65c3"


#setup 1wire sensor interface
os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')

def read_temp_raw(sensor_dir):
    device_file = sensor_dir + '/w1_slave'
    try:
        f = open(device_file, 'r')
    except IOError:
        #print "IOError: cannot open file"
        return -1
    lines = f.readlines()
    f.close()
    return lines
 
def read_temp(sensor_dir):
    lines = read_temp_raw(sensor_dir)
    if lines == -1:
      return -1
    #while lines[0].strip()[-3:] != 'YES':
    #    time.sleep(0.2)
    #    lines = read_temp_raw(sensor_dir)
    if lines[0].strip()[-3:] != 'YES':
      return -1
    equals_pos = lines[1].find('t=')
    if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:]
        temp_c = float(temp_string) / 1000.0
        temp_f = temp_c * 9.0 / 5.0 + 32.0
        #return temp_c, temp_f
        return temp_f

# setup database
db_con = False

while db_con == False:
    try:
        con = lite.connect(SYSTEM_DB)
        cur = con.cursor()  
        cur.executescript("""
            CREATE TABLE IF NOT EXISTS hlt_current_pid_data(Id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER, HLT_current_temp REAL, HLT_current_PWM REAL, HLT_PID_Integrator REAL, HLT_PID_Derivator REAL);
            CREATE TABLE IF NOT EXISTS HLT_pid_settings(Id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER, PID_set_temp REAL, PID_P_val REAL, PID_I_val REAL, PID_D_val REAL, ENABLE_bool INTEGER);          
            INSERT INTO HLT_pid_settings (PID_set_temp, PID_P_val, PID_I_val, PID_D_val, ENABLE_bool) VALUES (0, 0, 0, 0, 1);
            """)
        con.commit()
        db_con = True
    except lite.Error, e:
        if con:
            con.rollback()
        print "Error %s:" % e.args[0]
        #sys.exit(1)
 
print "Database successfully created"

# setup RPi GPIO as PWM outputs
GPIO.setmode(GPIO.BCM)
GPIO.setup(23,GPIO.OUT) #for HLT PWM
HLT_PWM = GPIO.PWM(23,1) #PWM instance for HLT @ 1Hz
HLT_PWM.start(0)

# create PID for HLT temp control
HLT_PID = PID()

#get PID setpoint, P, I, D vals from database
#last_row = cur.lastrowid
cur.execute("SELECT * FROM HLT_pid_settings WHERE Id=(SELECT MAX(Id) FROM HLT_pid_settings)")
row = cur.fetchone()
#print row[2], row[3], row[4], row[5]
HLT_PID.setPoint(row[2])
HLT_PID.setKp(row[3])
HLT_PID.setKi(row[4])
HLT_PID.setKd(row[5])
HLT_enable = 1
HLT_PID.setMaxOutput(100)
HLT_PID.setMinOutput(0)
last_row = 0

while True:
#    print "PID loop"
#    print "PID settings: setpoint: %f P: %f I: %f D: %f" % (HLT_PID.getPoint(),HLT_PID.getP_val(),HLT_PID.getI_val(),HLT_PID.getD_val())
    # get current HLT temp from 1wire interface, and write it to sensor database
    HLT_cur_temp = read_temp(HLT_DIR)
    print "HLT current temp: %f" % HLT_cur_temp
    HLT_PID_Integrator_val = HLT_PID.getIntegrator()
    HLT_PID_Derivator_val = HLT_PID.getDerivator()
    print "Integrator: " + str(HLT_PID_Integrator_val)
    print "Derivator: " + str(HLT_PID_Derivator_val)

    # update pid and get new PWM output
    if HLT_enable == True:
        new_HLT_PWM_value = HLT_PID.update(HLT_cur_temp)
    else:
        new_HLT_PWM_value = 0 	       
        print "HLT PID is disabled"
    
    print "PID PWM value: " +  str(new_HLT_PWM_value)

    cur_time = time.time()
    cur.execute("INSERT INTO hlt_current_pid_data(date, HLT_current_temp, HLT_current_PWM, HLT_PID_Integrator, HLT_PID_Derivator) VALUES (?,?,?,?,?)", (cur_time, HLT_cur_temp, new_HLT_PWM_value, HLT_PID_Integrator_val, HLT_PID_Derivator_val))
    con.commit()

    HLT_PWM.ChangeDutyCycle(new_HLT_PWM_value)
#    cur.execute("INSERT INTO HLT_pid_settings(PID_current_PWM) VALUES (?) WHERE Id=(SELECT MAX(Id)", (new_HLT_PWM_value,))
#    con.commit()

    # check if any new PID settings have been made, if so update PID
        #check if latest entry in pid_settings is newer than the last one
    #new_lastrow = cur.lastrowid
    cur.execute('SELECT max(id) FROM HLT_pid_settings')
    new_lastrow = cur.fetchone()[0]
    if new_lastrow > last_row:
        print "PID settings changed, updating values..."
        print last_row, new_lastrow
        # if new, do the following:
        cur.execute("SELECT * FROM HLT_pid_settings WHERE Id=(SELECT MAX(Id) FROM HLT_pid_settings)")
        row = cur.fetchone()
        print row[2], row[3], row[4], row[5]
        HLT_PID.setPoint(row[2])
        HLT_PID.setKp(row[3])
        HLT_PID.setKi(row[4])
        HLT_PID.setKd(row[5])
#        print "Integrator: " + str(HLT_PID.getIntegrator())
#        print "Derivator: " + str(HLT_PID.getDerivator())
        HLT_enable = row[6]
        last_row = new_lastrow
    #else:
        #print "no new PID changes, on to next loop..."
    # sleep and wait to make next measurement/PID adjustment
    time.sleep(1)
