/*
Halloween Jump Prop
by: Nate Zaugg

Activated / On = 2ms
Deactivate/ Off = 1ms

Freq = 50 Hz
20ms between pulses

Channel 5 on the Ridio

Documentation: 
- Look in the readme. There is a bug in the web version of this where URLS get clicked when you click anyplace.

PIN A0 = Radio Input (Channel 5)
PIN 13 = Output Trigger
PIN 12 = Timer debug signal

TODO: Alias these pins with #DEFINE
*/

// Varible used by the interrupts
// NOTE: This is volatile because it is being used in an interrupt
volatile int loopValue = 0;

// Application Setup
void setup() 
{
  // Should be standard practice; just in case our program is funky!
  delay(1000); 
  
  // debug
  pinMode(12, OUTPUT);
  
  pinMode(A0, INPUT);   // Listen on A0
  pinMode(13, OUTPUT);  // LED /Output
  SetupPinChangeInterrupts(A0);
  SetupTimer();
}

// Main Loop
void loop() 
{
    // the goggles, they do nothing
}

// Setup the timer for 2KHz on TIMER0
void SetupTimer()
{
  cli();//stop interrupts
  
  //set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 124;// = (16*10^6 CPU Frequency) / (2000Khz Desired Frequency * 64 Prescailer) - 1 (must be <256)
  
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  
  sei();//resume interrupts
}

// TIMER0 Interupt Service Routine
ISR(TIMER0_COMPA_vect){  //change the 0 to 1 for timer1 and 2 for timer2
   //interrupt commands here
  ++loopValue;
  digitalWrite(12, loopValue % 2);
}

// Setup Pin Change Interrupts
void SetupPinChangeInterrupts(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

// D8-D13 Pin Change Interrupt Service Routine
ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
{
  
}

// A0-A5 Pin Change Interrupt Service Routine
ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
{
  //digitalWrite(13,digitalRead(A0));
  byte val = digitalRead(A0);
  
  // We won't know quite what to do until the pin goes low
  if (val == HIGH)
  {
    // Reset our counter with each new pulse
    loopValue = 0;
  }
  else
  {
    // if we have 3 or more pulses then we know we're more than 1ms
    if (loopValue >= 3)
      digitalWrite(13, HIGH);
    else
      digitalWrite(13, LOW);
  }
}  

// D0-D7 Pin Change Interrupt Service Routine
ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
{
  
}
