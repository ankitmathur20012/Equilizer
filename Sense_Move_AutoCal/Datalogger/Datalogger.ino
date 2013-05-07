#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);

#define GPSECHO  true

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;

void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

void setup()
{
 // Open Serial communications and wait for port to open:
  Serial.begin(115200);

  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_GGAONLY); 
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
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
   
      Serial.println(GPS.lastNMEA());

   }
}
