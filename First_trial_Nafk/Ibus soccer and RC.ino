/*
  RC Remote Car
  fsi6x-RC-car-spin.ino
  Uses Flysky FS-I6X RC receiver & FS-I6X 6-ch Controller
  Uses TB6612FNG H-Bridge Motor Controller
  Drives two DC Motors

  Two Drive Modes - Normal and Spin
  Map channel 6 on controller to switch SWA for mode control

  Right stick controls direction in Normal mode (CH1 & CH2)
  VRA controls speed and direction in Spin mode (CH5)
  Left stick is acceleration in both modes (CH3)

  Channel functions by Ricardo Paiva - https://gist.github.com/werneckpaiva/

  DroneBot Workshop 2021
  https://dronebotworkshop.com
*/

// Include iBus Library
#include <IBusBM.h>

// Create iBus Object
IBusBM ibus;

// Channel Values

int rcCH1 = 0; // Left - Right
int rcCH2 = 0; // Forward - Reverse
int rcCH3 = 0; // Acceleration
int rcCH5 = 0; // Spin Control
int rcCH6 = 0; // Mode Control
bool rcCH7 = 0;
// Properties of Left-sided motors
#define l_RPWM 5
#define l_LPWM 6
#define l_R_EN 7
#define l_L_EN 8

// Properties of Right-sided motors
#define r_RPWM 11
#define r_LPWM 10
#define r_R_EN 2
#define r_L_EN 3

/*
// Motor A Control Connections
#define pwmA 3
#define in1A 5
#define in2A 4

// Motor B Control Connections
#define pwmB 9
#define in1B 7
#define in2B 8
*/


// Motor Speed Values - Start at zero
int MotorSpeedA = 0;
int MotorSpeedB = 0;

// Motor Direction Values - 0 = backward, 1 = forward
int MotorDirA = 1;
int MotorDirB = 1;

// Control Motor A
void mControlA(int mspeed, int mdir) {

  // Determine direction
  if (mdir == 0) {
    // Motor backward
      analogWrite(l_RPWM, 0);
      analogWrite(l_LPWM, mspeed); //left side wheels
  } else {
    // Motor forward
      analogWrite(l_RPWM, mspeed);
      analogWrite(l_LPWM, 0); //left side wheels
  }

}

// Control Motor B
void mControlB(int mspeed, int mdir) {

  // Determine direction
  if (mdir == 0) {
    // Motor backward
      analogWrite(r_RPWM, 0);
      analogWrite(r_LPWM, mspeed);//Right side wheels
  } else {
    // Motor forward
      analogWrite(r_RPWM, mspeed);
      analogWrite(r_LPWM, 0);//Right side wheels
  }

}

// Read the number of a given channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue) {
  uint16_t ch = ibus.readChannel(channelInput);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

// Read the channel and return a boolean value
bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}

void setup()

{

  // Start serial monitor for debugging
  Serial.begin(115200);

  // Attach iBus object to serial port
  ibus.begin(Serial);

  // Set all the motor control pins to outputs
/*
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(in1A, OUTPUT);
  pinMode(in2A, OUTPUT);
  pinMode(in1B, OUTPUT);
  pinMode(in2B, OUTPUT);
*/
  pinMode(l_RPWM, OUTPUT);
  pinMode(l_LPWM, OUTPUT);
  pinMode(l_R_EN, OUTPUT);
  pinMode(l_L_EN, OUTPUT);
  
  pinMode(r_RPWM, OUTPUT);
  pinMode(r_LPWM, OUTPUT);
  pinMode(r_R_EN, OUTPUT);
  pinMode(r_L_EN, OUTPUT);


}

void loop() {

  // Get RC channel values
  rcCH6 = readChannel(5, 0, 255, 0);
  rcCH1 = readChannel(0, -1*(rcCH6), rcCH6, 0);
  //rcCH1 = readChannel(0, -100, 100, 0);
  rcCH2 = readChannel(1, -255, 255, 0);
  rcCH3 = readChannel(2, 0, 155, 0);
  rcCH5 = readChannel(4, 0, 100, 0);
  
  rcCH7 = readSwitch(7, false);

  // Print values to serial monitor for debugging
  Serial.print("Ch1 = ");
  Serial.print(rcCH1);

  Serial.print(" Ch2 = ");
  Serial.print(rcCH2);

  Serial.print(" Ch3 = ");
  Serial.print(rcCH3);

  Serial.print(" Ch5 = ");
  Serial.print(rcCH5);

  Serial.print(" Ch6 = ");
  Serial.println(rcCH6);

  Serial.print(" Ch7 = ");
  Serial.println(rcCH7);

  // Set speeds with channel 3 value
  //MotorSpeedA = rcCH3;
  //MotorSpeedB = rcCH3;
  MotorSpeedA = 0;
  MotorSpeedB = 0;

  digitalWrite(l_R_EN, HIGH);
  digitalWrite(l_L_EN, HIGH);
  digitalWrite(r_R_EN, HIGH);
  digitalWrite(r_L_EN, HIGH);
      

    // Set forward/backward direction with channel 2 value
    if (rcCH2 >= 0) {
      //Forward
      MotorDirA = 1;
      MotorDirB = 1;
      Serial.println("Forward");
    } else {
      //Backward
      MotorDirA = 0;
      MotorDirB = 0;
      Serial.println("Backward");
    }

    // Add channel 2 speed
    MotorSpeedA = MotorSpeedA + abs(rcCH2);
    MotorSpeedB = MotorSpeedB + abs(rcCH2);

    // Set left/right offset with channel 1 value
    MotorSpeedA = MotorSpeedA + rcCH1;
    MotorSpeedB = MotorSpeedB - rcCH1;

 

  // Ensure that speeds are between 0 and 255
  MotorSpeedA = constrain(MotorSpeedA, 0, 255);
  MotorSpeedB = constrain(MotorSpeedB, 0, 255);
if (rcCH1 > 0 &&  rcCH2 > -1*(rcCH5) && rcCH2 < rcCH5 ) {
      //Motor A
      analogWrite(l_RPWM, MotorSpeedA);
      analogWrite(l_LPWM, 0); //left side wheels

      //Motor B
      analogWrite(r_RPWM, 0);
      analogWrite(r_LPWM, MotorSpeedA);//Right side wheels
      Serial.println("Clock-wise");
      
    } else if(rcCH1 < 0 &&  rcCH2 > -1*(rcCH5) && rcCH2 < rcCH5){
      //Motor A
      analogWrite(l_RPWM, 0);
      analogWrite(l_LPWM, MotorSpeedB); //left side wheels

      //Motor B
      analogWrite(r_RPWM, MotorSpeedB);
      analogWrite(r_LPWM, 0);//Right side wheels
      
      
      Serial.println("AntiCloc-wise");
    }else{
      //Drive Motors
      mControlA(MotorSpeedA, MotorDirA);
      mControlB(MotorSpeedB, MotorDirB);
    
      Serial.println("Regular");
    }
  //Drive Motors
  //mControlA(MotorSpeedA, MotorDirA);
  //mControlB(MotorSpeedB, MotorDirB);

  // Print speed values to serial monitor for debugging
  Serial.print("Motor A Speed = ");
  Serial.print(MotorSpeedA);
  Serial.print(" | Motor B Speed = ");
  Serial.println(MotorSpeedB);

  // Slight delay
  delay(50);

}
