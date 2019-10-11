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
            CREATE TABLE IF NOT EXISTS brewday_log(Id INTEGER PRIMARY KEY AUTOINCREMENT, PreBoilGravity VARCHAR, GroundWaterTemp FLOAT, MashpH FLOAT);
            """)
        con.commit()
        db_con = True
    except lite.Error, e:
        if con:
            con.rollback()
        print "Error %s:" % e.args[0]
        #sys.exit(1)
 
print "Database successfully created"
