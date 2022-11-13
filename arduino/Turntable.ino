/*
** Turntable control
** (C) Copyright 2022 by bunnyology
** https://github.com/bunnyology/Turntable.git
** --------------------------------------------------------
** Monophasic stepper motor 28-BYJ-48 with ULN2003
** driver (4 channels). 8 half steps.
** Wiring:
**   GPIO    MOTOR  ULN2003
**    D8  ->   B'    (IN1)
**    D9  ->   A     (IN2)
**    D10 ->   B     (IN3)
**    D11 ->   A'    (IN4)
**
** Linear/sliding potentiometer to control the speed
** connected to A0.
**
*/
#define STEPPER_MASK B11110000
#define STEPPER_STEPS 8

byte stepPattern[] = { 
  B00001000, 
  B00001100, 
  B00000100, 
  B00000110, 
  B00000010, 
  B00000011, 
  B00000001, 
  B00001001 
};
int step = 0;


void setup() {
  // analog input A0 to read potentiometer
  pinMode(A0, INPUT);
  // D8..D11 : OUTPUT
  DDRB = B00001111;
  PORTB = ( PORTB & STEPPER_MASK );   // all off
}

void loop() {
  // read potentiometer (10 bit -> 0...1023)
  int n = analogRead(A0);
  int dir;
  if( n < 512 ) {
    // slider on the left -> counter-clockwise
    dir = -1;
    n = 511 - n;
  } else {
    // slider on the right -> clockwise
    dir = 1;
    n -= 512;
  }
  
  // convert reading into number between 0..10
  int spd = n/48;
  if( spd == 0 ) {
    // lever in the middle -> power off
    PORTB = ( PORTB & STEPPER_MASK );   // all off
    delay(100);
    
  } else {
    // set stepper pattern according to current step
    PORTB = ( PORTB & STEPPER_MASK ) | stepPattern[step];
    
    // prepare next step
    step += dir;
    if( step < 0 ) step = STEPPER_STEPS;
    else if( step >= STEPPER_STEPS ) step = 0;
    
    // delay stepping according to potentiometer setting
    // between 12000 us (slow) and 1200 us (fast)
    delayMicroseconds(12000 / spd);
  }
}
