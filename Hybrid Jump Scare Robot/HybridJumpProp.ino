/*
Hybrid Halloween Jump Prop
by: Dan Beus & Nate Zaugg

Documentation: 
- Look in the readme. There is a bug in the web version of this where URLS get clicked when you click anyplace.

PIN D8 = Radio Input Manual Toggle (Channel 5)
PIN D2 = Radio Input Enable / Disable (Channel 6)
PIN A1 = Output Trigger
PIN A0 = Timer debug signal
PIN A4 = I2C - SDA
PIN A5 = I2C - SCL

*/
#include <Wire.h>
#include <VL53L1X.h>

// Remote Control Pins
#define ManualToggleInPin 8 // D8
#define EnableToggleInPin 2 // D2
#define OutPin A1
#define DebugPin A0 // A0

// Distance Sensor Constants
#define AVERAGE_SIZE 5
#define MINIMUM_WAIT 60000
#define RESET_OFFSET 6000

// Varible used by the interrupts
// NOTE: This is volatile because it is being used in an interrupt
volatile int ManualTriggerLoopValue = 0;
volatile int EnableTriggerLoopValue = 0;
volatile bool TriggerEnabled = false;
volatile long CurrentTime = 0;

VL53L1X sensor;
uint16_t rollingAverage[AVERAGE_SIZE];
uint8_t index = 0;
long previousJump = 0;
bool hasJumped = false;

// Forward Declairation
uint16_t Average();
void Jump(bool reset);

// *********************************************************** //
// Application Setup
// *********************************************************** //
void setup() 
{
  // Should be standard practice; just in case our program starts causing issues!
  delay(1000);
  
  pinMode(ManualToggleInPin, INPUT);
  pinMode(EnableToggleInPin, INPUT);
  pinMode(OutPin, OUTPUT);
  digitalWrite(OutPin, LOW);

  // debug
  pinMode(DebugPin, OUTPUT);
  Serial.begin(115200);

  SetupPinChangeInterrupts(ManualToggleInPin);
  SetupPinChangeInterrupts(EnableToggleInPin);

  // Setup Timer Interrupts
  SetupTimer();

  // Setup I2C for Distance Sensor
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

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

// *********************************************************** //
// Main Loop
// *********************************************************** //
void loop() 
{
  if(hasJumped)
  {
    if(CurrentTime > previousJump + RESET_OFFSET)
    {
      Jump(false);
      hasJumped = false;
      previousJump = CurrentTime;
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

      if (CurrentTime % 10 == 0)
      {
        Serial.print("Current reading: ");      
        Serial.print(distance);
        Serial.print("; Current time: ");
        Serial.print(CurrentTime);
        Serial.print("; Previous time: ");
        Serial.print(previousJump);
        Serial.print("; Trigger: ");
        Serial.println(TriggerEnabled);
      }

      rollingAverage[index] = distance;
      uint16_t average = Average();

      ++index %= AVERAGE_SIZE;
    
      if(CurrentTime > previousJump + MINIMUM_WAIT && average > 1000 && average < 1500 && TriggerEnabled)
      {
        Jump(true);
        previousJump = CurrentTime;
        hasJumped = true;
      }
    }
    else
    {
      Serial.println("Invalid Range!");
    }
    
  }
}

// *********************************************************** //
// Calculate the rolling average of the distance readings.
// *********************************************************** //
uint16_t Average()
{
  uint16_t sum = 0;
  for(uint8_t i = 0; i < AVERAGE_SIZE; ++i)
  {
    sum += rollingAverage[index];
  }

  return sum / AVERAGE_SIZE;
}

// *********************************************************** //
// Trigger the prop to jump or reset
// *********************************************************** //
void Jump(bool trigger)
{
  if (trigger && TriggerEnabled)
  {
    digitalWrite(OutPin, HIGH);
    Serial.print("************ JUMP ");
    Serial.print(trigger);
    Serial.println(" **********");
  }
  else
  {
    digitalWrite(OutPin, LOW);
  }  
}

// *********************************************************** //
// Setup the timer for 2KHz on TIMER0
// *********************************************************** //
void SetupTimer()
{
  cli();//stop interrupts
  
  //set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0

  // set compare match register for 2khz increments
  OCR0A = 124;// = (16*10^6 CPU Frequency) / (2000Hz Desired Frequency * 64 Prescailer) - 1 (must be <256)
  
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  
  sei();//resume interrupts
}

// *********************************************************** //
// TIMER0 Interupt Service Routine
// 
//change the TIMER0_COMPA_vect to TIMER1_COMPA_vect for timer1 and 2 for timer2
// *********************************************************** //
ISR(TIMER0_COMPA_vect){
  ++CurrentTime;
  ++ManualTriggerLoopValue;
  ++EnableTriggerLoopValue;
  //digitalWrite(DebugPin, ManualTriggerLoopValue % 2);
  //digitalWrite(DebugPin, EnableTriggerLoopValue % 2);
}

// *********************************************************** //
// Setup Pin Change Interrupts
// *********************************************************** //
void SetupPinChangeInterrupts(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

// *********************************************************** //
// D8-D13 Pin Change Interrupt Service Routine
// 
// handle pin change interrupt for D8 to D13 here
// *********************************************************** //
ISR (PCINT0_vect) 
{
  byte val = digitalRead(ManualToggleInPin);
  
  // We won't know quite what to do until the pin goes low
  if (val == HIGH)
  {
    // Reset our counter with each new pulse
    ManualTriggerLoopValue = 0;
  }
  else
  {
    // if we have 3 or more pulses then we know we're more than 1ms
    Jump(ManualTriggerLoopValue >= 3);
  }
}

// *********************************************************** //
// A0-A5 Pin Change Interrupt Service Routine
//
// Handle pin change interrupt for A0 to A5 here
// *********************************************************** //
ISR (PCINT1_vect)
{
  // Not used at this point, but could be helpful sometime in the future 
}  

// *********************************************************** //
// D0-D7 Pin Change Interrupt Service Routine
//
// Handle pin change interrupt for D0 to D7 here
// *********************************************************** //
ISR (PCINT2_vect) 
{
  //Serial.println("Enable Interrupt...");
  byte val = digitalRead(EnableToggleInPin);
  
  // We won't know quite what to do until the pin goes low
  if (val == HIGH)
  {
    // Reset our counter with each new pulse
    EnableTriggerLoopValue = 0;
  }
  else
  {
    // if we have 3 or more pulses then we know we're more than 1ms
    bool oldValue = TriggerEnabled;
    TriggerEnabled = EnableTriggerLoopValue >= 3;

    if (oldValue != TriggerEnabled)
    {
      Serial.print("Trigger: ");
      Serial.println(TriggerEnabled);
    }
  }
  //Serial.println("End Enable Interrupt...");
}
