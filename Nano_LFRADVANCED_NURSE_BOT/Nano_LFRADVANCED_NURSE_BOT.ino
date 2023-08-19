#include <Servo.h>

// Define the pins for the IR sensors
const int IR1 = A0;
const int IR2 = A1;
const int IR3 = A2;
const int IR4 = A3;
const int IR5 = A4;

// Define the pins for the L298N motor driver
#define ENA 9
#define IN1 5
#define IN2 6
#define IN3 7
#define IN4 8
#define ENB 10

// Define the pin for the servo
#define SERVO_PIN 3

Servo doorServo;  // Create a servo object

void setup() {
  Serial.begin(9600);

  // Initialize the IR sensor pins as inputs
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);

  // Initialize the motor driver pins as outputs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Attach the servo
  doorServo.attach(SERVO_PIN);
  doorServo.write(0);  // Assume door is closed initially
}

void loop() {
  // Read the values from the IR sensors
  bool ir1 = digitalRead(IR1);
  bool ir2 = digitalRead(IR2);
  bool ir3 = digitalRead(IR3);
  bool ir4 = digitalRead(IR4);
  bool ir5 = digitalRead(IR5);
  Serial.print(ir1);
  Serial.print("  ");
  Serial.print(ir2);
  Serial.print("  ");
  Serial.print(ir3);
  Serial.print("  ");
  Serial.print(ir4);
  Serial.print("  ");
  Serial.print(ir5);
  Serial.println("  ");
  // Implement the line following algorithm
  if (!ir1 && !ir2 && ir3 && !ir4 && !ir5) {
    // Go straight
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (ir1 || (ir1 && ir2)) {
    // Hard left
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (ir2 && !ir3) {
    // Soft left
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (ir4 && !ir3) {
    // Soft right
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else if (ir5 || (ir4 && ir5)) {
    // Hard right
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    // Stop and open the door
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    doorServo.write(90);  // Open the door (adjust angle as needed)
    delay(5000);  // Wait for 5 seconds
    doorServo.write(0);  // Close the door (adjust angle as needed)
  }

  // Set the speed of the motors
  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
}
