import time
import datetime
import os
import sys
import glob
import sqlite3 as lite
from time import sleep
#import RPi.GPIO as GPIO

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
            CREATE TABLE IF NOT EXISTS checklist_times(Id INTEGER PRIMARY KEY AUTOINCREMENT, StartTime_timestamp_local VARCHAR, StartTime_timestamp_epoch INTEGER, FillHLT_timestamp_local VARCHAR, FillHLT_timestamp_epoch INTEGER, HeatStrikeWater_timestamp_local VARCHAR, HeatStrikeWater_timestamp_epoch INTEGER, TransferStrikeWater_timestamp_local VARCHAR, TransferStrikeWater_timestamp_epoch INTEGER, DoughIn_timestamp_local VARCHAR, DoughIn_timestamp_epoch INTEGER, Mash_timestamp_local VARCHAR, Mash_timestamp_epoch INTEGER, MashOut_timestamp_local VARCHAR, MashOut_timestamp_epoch INTEGER, BatchSparge1_timestamp_local VARCHAR, BatchSparge1_timestamp_epoch INTEGER, Vorlauf1_timestamp_local VARCHAR, Vorlauf1_timestamp_epoch INTEGER, BatchSparge2_timestamp_local VARCHAR, BatchSparge2_timestamp_epoch INTEGER, Vorlauf2_timestamp_local VARCHAR, Vorlauf2_timestamp_epoch INTEGER, Boil_timestamp_local VARCHAR, Boil_timestamp_epoch INTEGER, Whirlpool_timestamp_local VARCHAR, Whirlpool_timestamp_epoch INTEGER, Chill_timestamp_local VARCHAR, Chill_timestamp_epoch INTEGER, Clean_timestamp_local VARCHAR, Clean_timestamp_epoch INTEGER);
            """)
        con.commit()
        db_con = True
    except lite.Error, e:
        if con:
            con.rollback()
        print "Error %s:" % e.args[0]
        #sys.exit(1)
 
print "Database successfully created"
