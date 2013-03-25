#!/usr/bin/python

import sys
import fileinput

GPRMC_file = open('GPRMC.log', "wb")
GPGGA_file = open('GPGGA.log', "wb")

def processFile(ifilename):
   
    # Constants: data field indices
    I_MSG_TYPE        = 0
        
    logfile = open(ifilename, "r")
    line = logfile.readline()
    print "Parsing " + ifilename + "..."
    
    parse_line = {"$GPRMC" : GPRMC, "$GPGGA" : GPGGA}

    for line in iter(lambda: logfile.readline().strip(),""):
        fields = line.split(',')
        parse_line[fields[I_MSG_TYPE]](fields)

def GPRMC(fields):

	I_MSG_TYPE		= 0
	I_GPS_TIME		= 1
	I_STATUS		= 2
	I_LATITUDE		= 3
	I_LATITUDE_DES	= 4
	I_LONGITUDE		= 5
	I_LONGITUDE_DES	= 6
	I_SPEED_KNOTS	= 7

	global GPRMC_file

	if (fields[I_STATUS] == 'A'):
		outstring = fields[I_GPS_TIME] + ',' + fields[I_STATUS] + ',' + fields[I_LATITUDE] + fields[I_LATITUDE_DES] + ',' + fields[I_LONGITUDE] + fields[I_LONGITUDE_DES] + ',' + fields[I_SPEED_KNOTS]
		GPRMC_file.write(outstring + '\n')

	return

def GPGGA(fields):

	I_MSG_TYPE			= 0
	I_GPS_TIME			= 1
	I_LATITUDE			= 2
	I_LATITUDE_DES		= 3
	I_LONGITUDE			= 4
	I_LONGITUDE_DES		= 5
	I_FIX_QUALITY		= 6
	I_NUM_SATELLITES	= 7
	I_HDOP				= 8

	global GPGGA_file

	if (fields[I_FIX_QUALITY] != '0'):
		outstring = fields[I_GPS_TIME] + ',' + fields[I_LATITUDE] + fields[I_LATITUDE_DES] + ',' + fields[I_LONGITUDE] + fields[I_LONGITUDE_DES] + ',' + fields[I_FIX_QUALITY] + ',' + fields[I_NUM_SATELLITES] + ',' + fields[I_HDOP]
		GPGGA_file.write(outstring + '\n')

	return

processFile(sys.argv[1])