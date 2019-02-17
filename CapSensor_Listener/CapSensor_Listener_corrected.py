import serial


def Level_Based_On_LookupTable(currentTemp, rawLevelData):
    if currentTemp <= 60:
        current_lookup_array = 60f_array
        for int i = 0;i<volume_array.buffer_info()[1];i++:
            if rawLevelData <= 60f_array[i]:
              # found volume
                return volume_array[i]
    else if currentTemp > 60 and currentTemp <= 83:
      # extrapolate between 60 and 83 array
        scaleFactor = (currentTemp - 60.0) / (83.0 - 63.0)
        for int i = 0;i<volume_array.buffer_info()[1];i++:
            extrapolated_value = (1-scaleFactor)*60f_array[i] + scaleFactor*83f_array[i]
            if rawLevelData <= extrapolated_value:
              # found volume
                return volume_array[i]
            
    else if currentTemp > 83 and currentTemp <= 107:
      # extrapolate between 83 and 107 array
        scaleFactor = (currentTemp - 83.0) / (107.0 - 83.0)
        for int i = 0;i<volume_array.buffer_info()[1];i++:
            extrapolated_value = (1-scaleFactor)*83f_array[i] + scaleFactor*107f_array[i]
            if rawLevelData <= extrapolated_value:
              # found volume
                return volume_array[i]
    else if currentTemp > 107 and currentTemp <= 127:
      # extrapolate 107 and 123
        scaleFactor = (currentTemp - 107.0) / (123.0 - 107.0)
        for int i = 0;i<volume_array.buffer_info()[1];i++:
            extrapolated_value = (1-scaleFactor)*107f_array[i] + scaleFactor*127f_array[i]
            if rawLevelData <= extrapolated_value:
              # found volume
                return volume_array[i]
    else if currentTemp > 127 and currentTemp <= 152:
      # extrapolate 123 and 152
        scaleFactor = (currentTemp - 123.0) / (152.0 - 123.0)
        for int i = 0;i<volume_array.buffer_info()[1];i++:
            extrapolated_value = (1-scaleFactor)*123f_array[i] + scaleFactor*152f_array[i]
            if rawLevelData <= extrapolated_value:
              # found volume
                return volume_array[i]
    else if currentTemp > 152 and currentTemp <= 212:
      # extrapolate 152 and 212
        scaleFactor = (currentTemp - 152.0) / (212.0 - 152.0)
        for int i = 0;i<volume_array.buffer_info()[1];i++:
            extrapolated_value = (1-scaleFactor)*152f_array[i] + scaleFactor*212f_array[i]
            if rawLevelData <= extrapolated_value:
              # found volume
                return volume_array[i]
    else if currentTemp > 212:
        for int i = 0;i<volume_array.buffer_info()[1];i++:
            if rawLevelData <= 212f_array[i]:
              # found volume
                return volume_array[i]

        


port = serial.Serial("/dev/ttyAMA0", baudrate=9600, timeout=3.0)

min_level_empty = 1225.0
max_level_50f = 3190.0

volume_array = array('f', [0.0, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5, 10.0, 10.5, 11.0, 11.5, 12.0, 12.5, 13.0, 13.5, 14.0])
60f_array = array('I', [1225, 1310, 1400, 1485, 1550, 1575, 1660, 1750, 1835, 1925, 2010, 2100, 2175, 2220, 2275, 2360, 2405, 2495, 2580, 2640, 2710, 2780, 2840, 2930, 3000, 3060, 3105]) 
83f_array = array('I', [1225, 1310, 1400, 1485, 1575, 1660, 1750, 1835, 1925, 2010, 2100, 2185, 2275, 2360, 2445, 2495, 2603, 2680, 2755, 2840, 2875, 2930, 3015, 3185, 3250, 3280, 3370])
107f_array = array('I', [1225, 1310, 1400, 1485, 1620, 1700, 1750, 1870, 1970, 2055, 2140, 2230, 2320, 2450, 2495, 2580, 2675, 2755, 2845, 2940, 3020, 3090, 3110, 3200, 3280, 3370, 3455, 3540])
127f_array = array('I', [1225, 1310, 1400, 1485, 1620, 1745, 1835, 1925, 2015, 2100, 2200, 2305, 2380, 2455, 2515, 2580, 2660, 2745, 2795, 2845, 2930, 3015, 3100, 3180, 3250, 3315, 3400, 3485])
152f_array = array('I', [1225, 1310, 1400, 1485, 1620, 1662, 1706, 1750, 1837, 1924, 1989, 2053, 2142, 2230, 2315, 2400, 2448, 2495, 2578, 2661, 2741, 2820, 2893, 2965, 3048, 3130, 3205, 3280])


while True:
    rcv = port.read(5)
    print "received raw serial data: %s" % rcv
    #compute fill level
    delta_raw_reading = int(rcv) - min_level_empty
    #print "delta raw: %f" %delta_raw_reading
    if delta_raw_reading < 0:
        delta_raw_reading = 0
    fill_level = delta_raw_reading / (max_level_50f - min_level_empty) 
    if fill_level > 1:
        fill_level = 1
    print "Fill Level: %f percent\n" % (fill_level*100)
    print "Gallons: %f gallons\n" % (fill_level*14)

