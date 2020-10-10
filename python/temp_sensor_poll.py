import time
import datetime
import os
import sys
import glob
import sqlite3 as lite
from time import sleep

#globals
con="" #db connection
SYSTEM_DB="/DB/brewing_db1.db" #location of sqlite db file
base_dir = '/sys/bus/w1/devices/' #location of 1wire sensor data
MASH_DIR = "/sys/devices/w1_bus_master1/28-000004cd6f23"
BOIL_DIR = "/sys/devices/w1_bus_master1/28-000004ce4807"
CHILL_DIR = "/sys/devices/w1_bus_master1/28-000004cea439"

enable = 1

#setup 1wire sensor interface
os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')

def read_temp_raw(sensor_dir):
    device_file = sensor_dir + '/w1_slave'
    #print "about to open file"
    try:
        f = open(device_file, 'r')
    except IOError:
        #print "IOError: cannot open file"
        return -1
    except:
        #print "generic error"
        return -1
    lines = f.readlines()
    #print "readlines completed"
    f.close()
    #print "file closed"
    return lines
 
def read_temp(sensor_dir):
    #print "read_temp"
    lines = read_temp_raw(sensor_dir)
    #print "lines returned: " + str(lines)
    if lines == -1:
      return -1
    #while lines[0].strip()[-3:] != 'YES':
    #    print "invalid temp read, trying again"
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
            CREATE TABLE IF NOT EXISTS mash_temp_sensor_data(Id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER, mash_current_temp REAL);
            CREATE TABLE IF NOT EXISTS boil_temp_sensor_data(Id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER, boil_current_temp REAL);
            CREATE TABLE IF NOT EXISTS chill_temp_sensor_data(Id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER, chill_current_temp REAL);
            """)
        con.commit()
        db_con = True
    except lite.Error, e:
        if con:
            con.rollback()
        print "Error %s:" % e.args[0]
        #sys.exit(1)
 
print "Database successfully created"

while enable:
    # get current temps from 1wire interface, and write it to sensor database
    mash_cur_temp = read_temp(MASH_DIR)
    boil_cur_temp = read_temp(BOIL_DIR)
    chill_cur_temp = read_temp(CHILL_DIR)
    print "Mash current temp: %f" % mash_cur_temp
    print "Boil current temp: %f" % boil_cur_temp
    print "Chill current temp: %f" % chill_cur_temp

    cur_time = time.time()
    try:
        cur.execute("INSERT INTO mash_temp_sensor_data(date, mash_current_temp) VALUES (?,?)", (cur_time, mash_cur_temp))
        cur.execute("INSERT INTO boil_temp_sensor_data(date, boil_current_temp) VALUES (?,?)", (cur_time, boil_cur_temp))
        cur.execute("INSERT INTO chill_temp_sensor_data(date, chill_current_temp) VALUES (?,?)", (cur_time, chill_cur_temp))
        con.commit()
	print "stored temp sensor values into db"
    except lite.Error, e:
        if con:
            con.rollback()
        print "Error %s:" % e.args[0]
        #if dropped db connection, attempt to reconnect
        con = lite.connect(SYSTEM_DB)

    time.sleep(1)
