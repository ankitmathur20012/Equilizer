#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include "Accelerometer.h"

Accelerometer myAccelerometer = Accelerometer();
SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);

#define GPSECHO  true

boolean initial_fix = false;
const int chipSelect = 4;

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;

void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

int file_inx = 0;
char filename[] = "datalog.txt";

void setup()
{
 // Open Serial communications and wait for port to open:
  Serial.begin(115200);
   while (!Serial) {
    ; // wait for dataFile port to connect. Needed for Leonardo only
  }
  //Connect up the following pins and your power rail
  //                    SL GS 0G   X  Y  Z
  myAccelerometer.begin(4, NULL, NULL, A0, A1, A2);

  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
 
  while(1) {
    if (SD.exists(filename)) {
      Serial.print(filename); Serial.println(" exists already");
      if (file_inx < 9) {
        file_inx++;
      }
      else {
        sprintf(filename,"datalog%d.txt",file_inx);
        break;
      }
      sprintf(filename,"datalog%d.txt",file_inx);
    }
    else {
      break;
    }  
  }   
  
  delay(1000); 

  Serial.println("Please place the Accelerometer on a flat\nLevel surface");
  delay(2000);//Give user 2 seconds to comply
  myAccelerometer.calibrate();


  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_GGAONLY); 
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  GPS.sendCommand(PMTK_ENABLE_SBAS);
  GPS.sendCommand(PMTK_API_SET_DGPS_MODE_WAAS);
  //GPS.sendCommand(PGCMD_ANTENNA);
  useInterrupt(true);
  delay(1000);

    
}


// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

int freeRam() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void loop()
{
  //myAccelerometer.read();    
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    // Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    //if (!GPS.parse(GPS.lastNMEA())){   // this also sets the newNMEAreceived() flag to false
    //  return;  // we can fail to parse a sentence in which case we should just wait for another
    //}
    File dataFile = SD.open(filename, FILE_WRITE); 
     
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(GPS.lastNMEA());
      //dataFile.print(",");
      //dataFile.print(myAccelerometer._Xgs);
      //dataFile.print(",");
      //dataFile.print(myAccelerometer._Ygs);
      //dataFile.print(",");
      //dataFile.println(myAccelerometer._Zgs);
      Serial.println(GPS.lastNMEA());
      //Serial.print(",");
      //Serial.print(myAccelerometer._Xgs);
      //Serial.print(",");
      //Serial.print(myAccelerometer._Ygs);
      //Serial.print(",");
      //Serial.println(myAccelerometer._Zgs);     
      dataFile.close();
    }  
        // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
   }
}
