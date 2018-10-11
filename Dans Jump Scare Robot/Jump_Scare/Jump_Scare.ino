/*
This example shows how to take simple range measurements with the VL53L1X. The
range readings are in units of mm.
*/

#include <Wire.h>
#include <VL53L1X.h>

#define AVERAGE_SIZE 5
#define MINIMUM_WAIT 30000
#define RESET_OFFSET 3000

const int ledPin = 3;

VL53L1X sensor;
uint16_t rollingAverage[AVERAGE_SIZE];
uint8_t index = 0;
long previousJump = 0;
bool hasJumped = false;

uint16_t Average();

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }
  
  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  sensor.startContinuous(50);
}

void loop()
{
  long currentTime = millis();

  if(hasJumped)
  {
    if(currentTime > previousJump + RESET_OFFSET)
    {
      Serial.println("Resetting prop");
      digitalWrite(ledPin, LOW);
      hasJumped = false;
      previousJump = currentTime;
    }
    else
    {
      Serial.println("Waiting for prop to reset");
    }
  }
  else
  {
    sensor.read();
  
    if(sensor.ranging_data.range_status == VL53L1X::RangeValid)
    {
      uint16_t distance = sensor.ranging_data.range_mm;
      Serial.print("Current reading: ");
      Serial.println(distance);
      
      rollingAverage[index] = distance;
      uint16_t average = Average();

      ++index %= AVERAGE_SIZE;
    
      if(currentTime > previousJump + MINIMUM_WAIT && average > 1000 && average < 1500)
      {
        Serial.println("Triggering prop");
        digitalWrite(ledPin, HIGH);
        previousJump = currentTime;
        hasJumped = true;
      }
      else
      {
        Serial.print("Current time: ");
        Serial.print(currentTime);
        Serial.print(";  Previous time: ");
        Serial.println(previousJump);
      }
    }
  }
}

uint16_t Average()
{
  uint16_t sum = 0;
  for(uint8_t i = 0; i < AVERAGE_SIZE; ++i)
  {
    sum += rollingAverage[index];
  }

  return sum / AVERAGE_SIZE;
}
