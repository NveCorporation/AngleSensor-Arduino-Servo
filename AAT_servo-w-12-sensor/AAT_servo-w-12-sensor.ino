/****************************************************************************** 
An NVE AAT003 angle sensor demonstration using a stepper motor servo and 
a proximity sensor to initialize the clock hand to the 12:00 position.
Rev. 1/21/16
******************************************************************************/
//Aurduino-to-stepper output definitions 
#define stp 2
#define dir 3
#define MS1 4
#define MS2 5
#define EN  6

//Constants
const float pi = 3.14159265;
const int steps = 200; //Stepper motor steps per revolution 
const int offset = 5; //Proximity sensor offset from 12:00 in steps  
const float deadband = 0.025; //Motion control deadband in radians

//Variables
int V0 = 512; //AAT output offset voltage (VDD=1024)
int AATsin = 0; //AAT signals
int AATcos = 0;
int x;
int noonish; //Inticates the hand is near the proximity sensor
float AATangle = 0.0; //AAT indicated angle in radians
float MotorAngle = 0.0; //Stepper motor angle in radians
float MotorStep = 2*pi/steps; //Step angle in radians
float error = 0.0; //Difference in motor position from sensor position

void setup() {
//Setup the proximity sensor interface
pinMode(8, INPUT); //Setup the proximity sensor input (IO8)
digitalWrite(8, HIGH); //Activate the IO8 input pull-up resistor

//Setup the blue LED to show the proximity sensor state
pinMode(13, OUTPUT); //Set the LED driver (IO13) as an output

//Setup the stepper motor driver interface
pinMode(stp, OUTPUT);
pinMode(dir, OUTPUT);
pinMode(MS1, OUTPUT);
pinMode(MS2, OUTPUT);
pinMode(EN, OUTPUT);

//Initialize the stepper motor driver
digitalWrite(MS1, LOW);  //MS1 and MS2 LOW for full steps 
digitalWrite(MS2, LOW);
digitalWrite(EN, LOW);  //Stepper driver LOW enable
digitalWrite(stp, LOW); //Intitialize stepper pulse

for(noonish = 1; noonish<2; noonish++) //Run twice if the hand's near 12:00
{
//If the hand is close to 12:00, move it to 6:00
 if(digitalRead(8) == LOW) //Read the sensor (LOW = on)
 {
    for(x= 1; x<steps/2; x++)
    {
      digitalWrite(dir,HIGH); //Step CW
      digitalWrite(stp,HIGH);
      delayMicroseconds(1000);
      digitalWrite(stp,LOW);
      delayMicroseconds(1000);
     }
 }
//Move the hand to the 12:00 position
 for(x= 1; x<steps; x++) 
 {
      digitalWrite(dir,HIGH); //Step CW
      delayMicroseconds(800);
      digitalWrite(stp,HIGH);
      delayMicroseconds(800);
      digitalWrite(stp,LOW);
      delayMicroseconds(800);
 if(digitalRead(8) == LOW) //Read the sensor (LOW = on)
  {
//Stop the motor when the proximity sensor turns on
  if (x <10) noonish = 0; //If x is small, the hand was close to 12:00, so do another iteration
  digitalWrite(13, HIGH); //Turn on the blue LED
  x=steps; // terminate loop
  }
 }
}
//Correct for the proximity sensor angular offset
for(x= 1; x<offset; x++) 
 {
      digitalWrite(dir,HIGH); //Step CW
      delayMicroseconds(800);
      digitalWrite(stp,HIGH);
      delayMicroseconds(800);
      digitalWrite(stp,LOW);
      delayMicroseconds(800);
      }
    delay(1000);
    }

//The main program loop
void loop() {
digitalWrite(13, !digitalRead(8)); //Blue LED shows proximity sensor state (low True)
AATcos = analogRead(0)-V0; //Read angle sensor
AATsin = analogRead(1)-V0;
AATangle = atan2(float(AATcos),float(AATsin)); //Calculate angle indicated by AAT sensor
error = MotorAngle - AATangle;

//Adjust the error to move the shortest direction
    if(error > pi) error = error - 2*pi;
    if(error < -pi) error = error + 2*pi;

//Rotate the motor to correct the error
    if(error < -deadband) //Motor behind
    { digitalWrite(dir,LOW); //Step CW
      delayMicroseconds(400);
      digitalWrite(stp,HIGH);
      delayMicroseconds(400);
      digitalWrite(stp,LOW);
      delayMicroseconds(400);
      MotorAngle = MotorAngle + MotorStep; //Increment by one step
      if(MotorAngle > 2*pi) MotorAngle = MotorAngle - 2*pi; //Keep angle <360
    }
    else if(error > deadband) //Motor ahead
    { digitalWrite(dir,HIGH); //Step CCW
      delayMicroseconds(400);
      digitalWrite(stp,HIGH); 
      delayMicroseconds(400);
      digitalWrite(stp,LOW);
      delayMicroseconds(400);
      MotorAngle = MotorAngle - MotorStep; //Decrement by one step
      if(MotorAngle < 0) MotorAngle = MotorAngle + 2*pi; //Keep angle positive
    }
  }
