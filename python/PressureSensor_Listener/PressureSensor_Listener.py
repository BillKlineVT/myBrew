import time
import datetime
import os
import sys
import glob
import serial
import sqlite3 as lite
import StringIO
import csv

#globals
con="" #db connection
SYSTEM_DB="/DB/brewing_db1.db" #location of sqlite db file
serial_select=0
enable=1
hlt_readings_queue = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
boil_readings_queue = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

port = serial.Serial("/dev/ttyAMA0", baudrate=9600, timeout=3.0)

min_level_empty = 1225.0
max_level_50f = 3190.0

def read_level_raw():
    try:    
	print "bytes in serial buffer: %d" % port.inWaiting() 
	if (port.inWaiting() > 10):
            port.flushInput()
	#while (port.inWaiting() < 5):
        #    print "waiting for next data"
	#    sleep(0.1)
        #rcv = port.read(5)
	rcv = port.readline()
        print "received raw serial data: %s" % rcv
    except IOError:
        #print "IOError: cannot open file"
        return -1
    except:
        #print "generic error"
        return -1

    if (rcv != ''):
	boil, hlt = rcv.split(',',1)
	print "valid value, returning boil: %s hlt: %s" % (boil, hlt)
        return boil,hlt
    else:
        print "empty value, returning -1"
	return -1


#while True:
    #rcv = port.read(5)
    #print "received raw serial data: %s" % rcv
    #current_boil_level = read_level_raw("boil")
    #current_hlt_level = read_level_raw("hlt")
    #compute fill level
    #delta_raw_reading = int(rcv) - min_level_empty
    #print "delta raw: %f" %delta_raw_reading
    #if delta_raw_reading < 0:
    #    delta_raw_reading = 0
    #fill_level = delta_raw_reading / (max_level_50f - min_level_empty) 
    #if fill_level > 1:
    #    fill_level = 1
    #print "Fill Level: %f percent\n" % (fill_level*100)
    #print "Gallons: %f gallons\n" % (fill_level*14)
    # setup database
db_con = False

while db_con == False:
    try:
        con = lite.connect(SYSTEM_DB)
        cur = con.cursor()
        cur.executescript("""
        CREATE TABLE IF NOT EXISTS boil_level_sensor_data(Id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER, boil_current_level INTEGER, calculated_volume FLOAT);
        CREATE TABLE IF NOT EXISTS hlt_level_sensor_data(Id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER, hlt_current_level INTEGER, calculated_volume FLOAT);
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
    cur_time = time.time()
    print "loop"
    current_boil_level=0
    current_hlt_level=0
    try:
        current_boil_level, current_hlt_level = read_level_raw()
    except:
        print "error on reading raw data"

    #push new data into queue
    hlt_readings_queue.insert(0,int(current_hlt_level))
    hlt_readings_queue.pop()
    new_avg_hlt_reading = sum(hlt_readings_queue)/len(hlt_readings_queue)

    boil_readings_queue.insert(0,int(current_boil_level))
    boil_readings_queue.pop()
    new_avg_boil_reading = sum(boil_readings_queue)/len(boil_readings_queue)


    print "boil_level being stored to db: %s hlt_level: %s" % (new_avg_boil_reading, new_avg_hlt_reading)
    #current_hlt_level = read_level_raw("hlt")
    #if(current_boil_level > 0):
    try:
        cur.execute("INSERT INTO boil_level_sensor_data(date, boil_current_level) VALUES (?,?)", (cur_time, new_avg_boil_reading))
        cur.execute("INSERT INTO hlt_level_sensor_data(date, hlt_current_level) VALUES (?,?)", (cur_time, new_avg_hlt_reading))
        con.commit()
    except lite.Error, e:
        if con:
            con.rollback()
        print "Error %s:" % e.args[0]
        #if dropped db connection, attempt to reconnect
        con = lite.connect(SYSTEM_DB)
    time.sleep(1)


