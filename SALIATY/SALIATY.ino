#include <Wire.h> //These two libraries are for the LCD
#include <LiquidCrystal_I2C.h>
#include <OneWire.h> //These two libraries are for the temp sensor
#include <DallasTemperature.h>

//Data wire from thermometer is plugged into digital pin 12 on the Arduino
#define ONE_WIRE_BUS 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

LiquidCrystal_I2C lcd(0x27, 16, 2); 

int analogPin = 0;
int dtime = 500;
int raw = 0;
int Vin = 5;
float Vout = 0;
float R1 = 1000;
float R2 = 0;
float buff = 0;
float avg = 0;
int samples = 5000;
int state = 0;
int button = 0;
int buttonPin = 5;

float CalibrationTemp = 21.0;  //This is the temperature from which calibration is done. Change if needed
float CorrectionFactor = 1.02;  //Correction factor found by us. Change if needed
float dTemp = 0;    //Initial value for temperature difference

//State 0 = rest
//State 1 = measure
//State 2 = print


void setup(){
  sensors.begin(); // start up temperature library
  pinMode(8,OUTPUT); // define ports 8 and 7 for AC 
  pinMode(7,OUTPUT);
  pinMode(12,INPUT);    //Setup the thermometerpin as input
  pinMode(buttonPin,INPUT); //Setup buttonpin as input
  Serial.begin(9600); // start serial

  lcd.begin(); //start up LCD
//  lcd.clear(); //clear LCD memory
  
}

void loop(){
  //REST
  button = digitalRead(buttonPin);
  if (button == HIGH) {   //Check is the button is pressed and go to another state to start measuring
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Starting");
    lcd.setCursor(2,1);
    lcd.print("measurement");
    state = 1;
  }
  
  
  //MEASURE
  if (state == 1) {
    float tot = 0;
    for (int i =0; i<samples; i++) {
      digitalWrite(7,HIGH);
      digitalWrite(8,LOW);
      delayMicroseconds(dtime);
      digitalWrite(7,LOW);
      digitalWrite(8,HIGH);
      delayMicroseconds(dtime);
      raw = analogRead(analogPin);
      if(raw){
        buff = raw * Vin;
        Vout = (buff)/1024.0;
        buff = (Vin/Vout) - 1;
        R2= R1 * buff;
        //Serial.print("Vout: ");
        //Serial.println(Vout);
        //Serial.print("R2: ");
        //Serial.println(R2);
        tot = tot + R2;
      }
    }
    avg = tot/samples;
    Serial.print("Average resistance is: ");
    Serial.print(avg);
    Serial.println(" Ohm");
    state = 2;

    // Send the command to get temperatures
    sensors.requestTemperatures(); 
    float temp = sensors.getTempCByIndex(0);
    //print the temperature in Celsius
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print("C  |  ");
    dTemp = temp - CalibrationTemp;
    avg = avg * pow(CorrectionFactor, dTemp);
    Serial.print("Corrected resistance is: " + String(avg));
  
  }

  //PRINT
  if (state == 2) { //Here the value found is printed and the values for different sorts of water are defined
    if (avg > 2000) {
      Serial.println("This is demineralised water.");
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Demi water");
      lcd.setCursor(1,1);
      lcd.print("AVG: " + String(avg));
      
      delay(1000);
      state = 0;
    }
    else if (avg > 1000) {
      Serial.println("This is fresh/tap water.");
      
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Tap water");
      lcd.setCursor(1,1);
      lcd.print("AVG: " + String(avg));
      
      delay(1000);
      state = 0;
    }
    else if (avg > 190) {
      Serial.println("This is brackish water.");
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Brackish water");
      lcd.setCursor(1,1);
      lcd.print("AVG: " + String(avg));
      
      delay(1000);
      state = 0;
    }
    else if (avg < 190) {
      Serial.println("This is seawater.");
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Sea water");
      lcd.setCursor(1,1);
      lcd.print("AVG: " + String(avg));
      
      delay(1000);
      state = 0;
    }
  }
}
