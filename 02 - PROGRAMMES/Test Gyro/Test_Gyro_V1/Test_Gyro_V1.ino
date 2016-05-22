#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_L3GD20_U.h>
#include <Servo.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);

float Position = 0.0L;
float Val = 0.0L;

void setup(void) 
{
  delay (2000);
  Serial.begin(9600);
  Serial.println("Gyroscope Test"); Serial.println("");
  
  /* Enable auto-ranging */
  gyro.enableAutoRange(true);
  
  /* Initialise the sensor */
  if(!gyro.begin(GYRO_RANGE_2000DPS))
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    Serial.println("Ooops, no L3GD20 detected ... Check your wiring!");
    while(1);
  }
  delay (1000);
}

void loop(void) 
{
  gyroscope();
}

void gyroscope()
{
  sensors_event_t event; 
  gyro.getEvent(&event);
  Val = event.gyro.z ;
  Position = Position + (Val+0.03)*0.01 ;
  delay(10);
}
