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
db_con = False

while db_con == False:
    # setup database
    try:
        con = lite.connect(SYSTEM_DB)
        cur = con.cursor()  
        cur.executescript("""
            CREATE TABLE IF NOT EXISTS gpio_outputs(Id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER, GPIO_18 INTEGER, GPIO_22 INTEGER, GPIO_24 INTEGER, GPIO_27 INTEGER);
            INSERT INTO gpio_outputs(GPIO_18, GPIO_22, GPIO_24, GPIO_27) VALUES (0, 0, 0, 0);
            """)
        con.commit()
        db_con = True
    except lite.Error, e:
        if con:
            con.rollback()
        print "Error %s:" % e.args[0]
        #sys.exit(1)
 
print "Database successfully created"

# setup RPi GPIO as outputs
GPIO.setmode(GPIO.BCM)
GPIO.setup(18,GPIO.OUT)
GPIO.setup(22,GPIO.OUT)
GPIO.setup(24,GPIO.OUT)
GPIO.setup(27,GPIO.OUT)

last_row=0

while 1:
    print "polling loop"
#    print "GPIO settings: GPIO_18: %d GPIO_22: %d GPIO_24: %d GPIO_27: %d" % (HLT_PID.getPoint(),HLT_PID.getP_val(),HLT_PID.getI_val(),HLT_PID.getD_val())

    cur.execute('SELECT max(id) FROM gpio_outputs')
    new_lastrow = cur.fetchone()[0]
    if new_lastrow > last_row:
        print "gpio settings changed, updating values..."
        print last_row, new_lastrow
        # if new, do the following:
        try:
            cur.execute("SELECT * FROM gpio_outputs WHERE Id=(SELECT MAX(Id) FROM gpio_outputs)")
            row = cur.fetchone()
            print row[2], row[3], row[4], row[5]
        except lite.Error, e:
            if con:
                con.rollback()
            print "Error %s:" % e.args[0]
            #if dropped db connection, attempt to reconnect
            con = lite.connect(SYSTEM_DB)
        
        gpio_18 = row[2]
        gpio_22 = row[3]
        gpio_24 = row[4]
        gpio_27 = row[5]
        GPIO.output(18,gpio_18)
        GPIO.output(22,gpio_22)
        GPIO.output(24,gpio_24)
        GPIO.output(27,gpio_27)
        last_row = new_lastrow
    else:
        print "no new gpio changes, on to next loop..."
    # sleep and wait to poll
    time.sleep(0.5)
