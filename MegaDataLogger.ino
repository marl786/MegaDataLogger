#include <Wire.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include <SFE_BMP180.h> 
//#include <SPI.h>
//#include <SD.h>

/******** HC-SR04 ~ parameters Start **********/
#define trigPin 10
#define echoPin 9
long    duration;
float   distance;
float   d = 0;
float   d1 = 0;
float   d2 = 0;
float   area = 0;
int     unitSelect = 0;
String  unit = "cm";
/******** HC-SR04 ~ parameters End **********/


/********* MPU6050 ~ parameters Start *********/
const int MPU = 0x68;
int16_t   AcX, AcY, AcZ;
int       axis = 0;
int       angle = 0;
/********* MPU6050 ~ parameters End *********/


/******** DHT22 ~ parameters Start **********/
#define DHTPIN 11
#define DHTTYPE DHT22
float   temp, c, f, k;
float   t = 0;
float   Humidity, h;
float   H, tC, tF;
float   HI = 0;
int     unitTempSelect = 0;
int     hif, hic;
String  unitTemp = "C";
/******** DHT22 ~ parameters End **********/


/********* Lux meter ~ parameters Start *********/
#define LDR_PIN                   A1
#define MAX_ADC_READING           1023
#define ADC_REF_VOLTAGE           5.0
#define REF_RESISTANCE            5030  // measure this for best results
#define LUX_CALC_SCALAR           12518931
#define LUX_CALC_EXPONENT         -1.405
float   resistorVoltage, ldrVoltage;
float   ldrResistance;
float   ldrLux;
float   lx;
float   l = 0; 
int     ldrRawData;
int     unitLuxSelect = 0;
String  unitLux = " ";  
/********* Lux meter ~ parameters End *********/


/********* BMP180 ~ parameters Start *********/
#define ALTITUDE 8 // Altitude of Karachi, pakistan in meters(Google)
char    status;
double  T,P,p0;
float   unitAltSelect = 0;
float   unitPresSelect = 0;
float   PRESSURE;
float   A,a;
String  unitalt = " ";
String  unitpre = " ";
/********* BMP180 ~ parameters End *********/


/********* Hall Effect Sensor ~ parameters Start *********/
char   ADCSHOW[5];//initializing a character of size 5 for showing the ADC result
float  magnet = A2;
String ADCVALUE;
/********* Hall Effect Sensor ~ parameters End *********/


/********* PIR-sensor ~ parameters Start *********/
#define ledPin 47
#define pirPin 22 
int     pirState = LOW; // we start, assuming no motion detected
int     val = 0; // variable for reading the pin status
int     counter = 0;
int     currentState = 0;
int     previousState = 0;
/********* PIR-sensor ~ parameters End *********/


/********** Capacitence meter ~ parameters Start **********/
#define       analogPin      A0          
#define       chargePin      12         
#define       dischargePin   13        
#define       resistorValue  10000.0F 
unsigned long startTime;
unsigned long elapsedTime;
float         microFarads;                
float         nanoFarads;
/********** Capacitence meter ~ parameters End **********/

/********** Resistor meter ~ parameters Start **********/
int analogpin= A3;
int raw= 0;
int Vin= 5;
float Vout= 0;
float R1= 1000;
float R2= 0;
float buffer= 0;
/********** Resistor meter ~ parameters End **********/

LiquidCrystal lcd(2,3,4,5,6,7); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
DHT dht(DHTPIN,DHTTYPE);
SFE_BMP180 pressure;

int program = 0;
int programbutton = 0;

/*Button*/
#define selectButton 8

void setup() {
  Serial.begin(9600);
  // Initialize interface to the MPU6050
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  lcd.begin(16, 2); // Initializes the interface to the LCD screen
  dht.begin();
  pressure.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(selectButton, INPUT_PULLUP);
  pinMode(chargePin, OUTPUT);     
  digitalWrite(chargePin, LOW);
}



void loop() {
  switch (program) {  // Switch between different programs
    case 0:  // Select unit of measurement
      lcd.setCursor(2, 0);
      lcd.print("For Distance ");
      lcd.setCursor(0, 1); // Sets the location at which subsequent text written to the LCD will be displayed
      lcd.print("Select Unit: ");
      lcd.setCursor(13, 1);
      lcd.print(unit);
      lcd.print("  ");
      delay(10);
      // If button is pressed - change unit
      if (digitalRead(selectButton) == 0) {
        if (unitSelect == 0) {
          unit = "in";
          unitSelect = 1;
        }
        else if (unitSelect == 1) {
          unit = "m";
          unitSelect = 2;
        }
        else if (unitSelect == 2) {
          unit = "ft";
          unitSelect = 3;
        }
        else if (unitSelect == 3) {
          unit = "cm";
          unitSelect = 0;
        }
        // If button is held longer then half a second - change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 1;
          lcd.clear();
          delay(500);
        }
      }
      break;
      
    case 1:   // Distance measuring program
      distance = getDistance(); // Distance to the nearest object
      lcd.setCursor(0, 0);
      lcd.print("Dist: ");
      lcd.print(distance); // Prints the distance value from the sensor
      lcd.print("    ");
      lcd.setCursor(14, 0);
      lcd.print(unit);
      delay(10);
      lcd.setCursor(0, 1);
      lcd.print("d:");
      lcd.setCursor(8, 1);
      lcd.print("d:");
      delay(200);
      // Save distance 1
      if (digitalRead(selectButton) == 0) {
        if (d == 0) {
          lcd.setCursor(0, 1);
          lcd.print("d:     ");
          lcd.setCursor(2, 1);
          lcd.print(distance);
          d = 1;
          delay(100);
        }
        // Save distance 2
        else if (d == 1) {
          lcd.setCursor(8, 1);
          lcd.print("d:     ");
          lcd.setCursor(10, 1);
          lcd.print(distance);
          d = 0;
          delay(100);
        }
        // If button is held longer then half sencond change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 2;
          d = 0;
          lcd.clear();
          delay(500);
        }
      }
      break;
      
    case 2:   // Area measuring program
      distance = getDistance();
      lcd.setCursor(0, 0);
      lcd.print("Area: ");
      lcd.print(area); // Prints the calculated area from the two measurements
      lcd.setCursor(12, 0);
      lcd.print(unit); // Prints the selected unit and the square sign below
      lcd.print("^2");
      delay(200);
      if ( d == 0) {
        lcd.setCursor(0, 1);
        lcd.print("d1:       ");
        lcd.setCursor(3, 1);
        lcd.print(distance);
        delay(200);
      }
      else if (d == 1) {
        lcd.setCursor(9, 1);
        lcd.print("d2:      ");
        lcd.setCursor(12, 1);
        lcd.print(distance);
        delay(200);
      }
      else if (d == 2) {
        lcd.setCursor(6, 0);
        lcd.print(area);
        delay(200);
      }
      // Save distance 1
      if (digitalRead(selectButton) == 0) {
        if (d == 0) {
          lcd.setCursor(0, 1);
          lcd.print("d1:    ");
          lcd.setCursor(3, 1);
          lcd.print(distance);
          d = 1;
          d1 = distance;
          delay(100);
        }
        // Save distance 2
        else if (d == 1) {
          lcd.setCursor(9, 1);
          lcd.print("d2:    ");
          lcd.setCursor(12, 1);
          lcd.print(distance);
          d = 2;
          d2 = distance;
          area = d1 * d2; // Calculate the area
          delay(100);
        }
        else if (d == 2) {
          lcd.clear();
          d = 0;
          area = 0;
          delay(100);
        }
        // If button is held longer then half sencond change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 3;
          d = 0;
          lcd.clear();
          delay(500);
        }
      }
      break;


      case 3:    // Angle measuring program
      // Read the accelerometer data
      Wire.beginTransmission(MPU);
      Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
      AcX = Wire.read() << 8 | Wire.read(); // X-axis value
      AcY = Wire.read() << 8 | Wire.read(); // Y-axis value
      AcZ = Wire.read() << 8 | Wire.read(); // Z-axis value
     
      if ( axis == 0) {
        // Calculating the Pitch angle (rotation around Y-axis)
        angle = atan(-1 * AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2))) * 180 / PI;
        lcd.setCursor(0, 0);
        lcd.print("Pitch");
      }
      else if (axis == 1) {
        // Calculating the Roll angle (rotation around X-axis)
        angle = atan(-1 * AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2))) * 180 / PI;
        lcd.setCursor(0, 0);
        lcd.print("Roll ");
      }
      lcd.setCursor(0, 1);
      lcd.print("Angle: ");
      lcd.print(abs(angle));
      lcd.print("     ");
      lcd.setCursor(10, 1);
      lcd.print("deg");
      delay(200);
      // Change axis
      if (digitalRead(selectButton) == 0) {
        if (axis == 0) {
          axis = 1;
          delay(100);
        }
        // Save distance 2
        else if (axis == 1) {
          axis = 0;
          delay(100);
        }
        // If button is held longer then half sencond change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 4;
          lcd.clear();
          delay(500);
        }
      }
        break;



        case 4: // Temperature measuring program
        lcd.setCursor(0, 0);
        lcd.print("For Temperature ");
        lcd.setCursor(0, 1); // Sets the location at which subsequent text written to the LCD will be displayed
        lcd.print("Select Unit: ");
        lcd.setCursor(13, 1);
        lcd.print(unitTemp);
        lcd.print("  ");
        delay(10);
        
        // If button is pressed - change unit
      if (digitalRead(selectButton) == 0) {
        if (unitTempSelect == 0) {
          lcd.write(8);
          unitTemp = "C";
          unitTempSelect = 1;
        }
        else if (unitTempSelect == 1) {
          lcd.write(8);
          unitTemp = "K";
          unitTempSelect = 2;
        }
        else if (unitTempSelect == 2) {
          unitTemp = "F";
          unitTempSelect = 0;
        }
        // If button is held longer then half a second - change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 5;
          lcd.clear();
          delay(500);
        }
      }
        break;

      case 5: //getting value of temperature.
        temp = getTemp();
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(temp); // Prints the distance value from the sensor
        lcd.print("    ");
        lcd.setCursor(14, 0);
        lcd.print(unitTemp);
        delay(10);
        lcd.setCursor(0, 1);
        lcd.print("T:");
        lcd.setCursor(8, 1);
        lcd.print("T:");
        delay(200);

        
        

        
        if(digitalRead(selectButton) == 0){
          if(t == 0){
            lcd.setCursor(0, 1);
            lcd.print("T:     ");
            lcd.setCursor(2, 1);
            lcd.print(temp);
            t = 1;
            delay(100);  
          }
          else if (t == 1){
            lcd.setCursor(8, 1);
            lcd.print("T:     ");
            lcd.setCursor(10, 1);
            lcd.print(temp);
            t = 0;
            delay(100);
          }
        // If button is held longer then half a second - change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 6;
          lcd.clear();
          delay(500);
        }
        }
         break;

        case 6: //getting values of humidity
          Humidity = getHumidity();
          lcd.setCursor(0, 0);
          lcd.print("Humidity: ");
          lcd.setCursor(10, 0);
          lcd.print(Humidity);
          lcd.setCursor(14,0);
          lcd.print("%");
          lcd.setCursor(0,1);
          lcd.print("UVIndex: ");
          h = Humidity/10;
          delay(100);

          

          
          if(h <= 0 and h >= 2){
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Humidity: ");
            lcd.setCursor(10, 0);
            lcd.print(Humidity);
            lcd.setCursor(14,0);
            lcd.print("%");
            lcd.setCursor(0,1);
            lcd.print("UVIndex: ");
            lcd.setCursor(9, 1);
            lcd.print("Low");
          }
          else if(h > 2 and h >= 5){
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Humidity: ");
            lcd.setCursor(10, 0);
            lcd.print(Humidity);
            lcd.setCursor(14,0);
            lcd.print("%");
            lcd.setCursor(0,1);
            lcd.print("UVIndex: ");
            lcd.setCursor(8, 1);
            lcd.print("Moderate");
          }
          else if(h > 5 and h >= 7){
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Humidity: ");
            lcd.setCursor(10, 0);
            lcd.print(Humidity);
            lcd.setCursor(14,0);
            lcd.print("%");
            lcd.setCursor(0,1);
            lcd.print("UVIndex: ");
            lcd.setCursor(8, 1);
            lcd.print("High");
          }
          else if(h > 7 and h >= 10){
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Humidity: ");
            lcd.setCursor(10, 0);
            lcd.print(Humidity);
            lcd.setCursor(14,0);
            lcd.print("%");
            lcd.setCursor(0,1);
            lcd.print("UVIndex: ");
            lcd.setCursor(8, 1);
            lcd.print("Extreme");
          }
          else{
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Humidity: ");
            lcd.setCursor(10, 0);
            lcd.print(Humidity);
            lcd.setCursor(14,0);
            lcd.print("%");
            lcd.setCursor(0,1);
            lcd.print("UVIndex: ");
            lcd.setCursor(8, 1);
            lcd.print("Danger");
          }
          // If button is held longer then half a second - change program
          delay(500);
          if (digitalRead(selectButton) == 0) {
            program = 7;
            lcd.clear();
            delay(500);
          }
          break;
        
        case 7: // Humidity
          H = getHumidity();
          tC = dht.readTemperature();
          tF = dht.readTemperature(true);
          lcd.setCursor(0, 0);
          lcd.print("Heat Index: ");
          hic = dht.computeHeatIndex(tC, H, false);
          lcd.setCursor(11,0);
          lcd.print(hic);
          lcd.print("C");
          delay(10);
          lcd.setCursor(0, 1);
          lcd.print("Heat Index: ");
          hif = dht.computeHeatIndex(tF, H);
          lcd.setCursor(11,1);
          lcd.print(hif);
          lcd.print("F");
          // If button is held longer then half a second - change program
          delay(500);
          if (digitalRead(selectButton) == 0) {
            program = 8;
            lcd.clear();
            delay(500);
          }
          break;

          case 8: // capacitence meter
          digitalWrite(chargePin, HIGH);  
          startTime = micros();
          while(analogRead(analogPin) < 648){       
          }

          elapsedTime= micros() - startTime;
          microFarads = ((float)elapsedTime / resistorValue) ;

          if (microFarads > 1){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("SCALE:  0.1uF-4F");
            lcd.setCursor(0,1);  
            lcd.print(microFarads);
            lcd.setCursor(14,1);        
            lcd.print("uF");   
            delay(500);    
          }
          else{
            nanoFarads = microFarads * 1000.0; 
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("SCALE:  0.1uF-4F");
            lcd.setCursor(0,1);       
            lcd.print(nanoFarads);  
            lcd.setCursor(14,1);       
            lcd.print("nF");          
            delay(500); 
          }
          digitalWrite(chargePin, LOW);            
          pinMode(dischargePin, OUTPUT);            
          digitalWrite(dischargePin, LOW);     //discharging the capacitor     
          while(analogRead(analogPin) > 0){         
          }//This while waits till the capaccitor is discharged   
          pinMode(dischargePin, INPUT);
          lcd.setCursor(0,1);
          // If button is held longer then half a second - change program
          delay(500);
          if (digitalRead(selectButton) == 0) {
            program = 9;
            lcd.clear();
            delay(500);
          }
          break;

          case 9: //Unit for illuminance
        lcd.setCursor(0, 0);
        lcd.print("For Illuminance");
        lcd.setCursor(0, 1); // Sets the location at which subsequent text written to the LCD will be displayed
        lcd.print("Select Unit:");
        lcd.setCursor(12, 1);
        lcd.print(unitLux);
        lcd.print("  ");
        delay(10);
        if (digitalRead(selectButton) == 0) {
        if (unitLuxSelect == 0) {
          unitLux = "Lux";
          unitLuxSelect = 1;
        }
        else if (unitLuxSelect == 1) {
          unitLux = "ftcd";
          unitLuxSelect = 0;
        }
        // If button is held longer then half a second - change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 10;
          lcd.clear();
          delay(500);
        }
        }
        break;

      case 10: // get lux
        ldrLux = luxmeter(); // Distance to the nearest object
        lcd.setCursor(0, 0);
        lcd.print("lx: ");
        lcd.print(ldrLux); // Prints the distance value from the sensor
        lcd.print("    ");
        lcd.setCursor(12, 0);
        lcd.print(unitLux);
        delay(10);
        lcd.setCursor(0, 1);
        lcd.print("lx:");
        lcd.setCursor(8, 1);
        lcd.print("lx:");
        delay(200);
        // Save lux 1
        if (digitalRead(selectButton) == 0) {
          if (l == 0) {
            lcd.setCursor(0, 1);
            lcd.print("lx:     ");
            lcd.setCursor(3, 1);
            lcd.print(ldrLux);
            l = 1;
            delay(100);
          }
          // Save lux 2
          else if (l == 1) {
            lcd.setCursor(8, 1);
            lcd.print("lx:     ");
            lcd.setCursor(11, 1);
            lcd.print(ldrLux);
            l = 0;
            delay(100);
          }
        // If button is held longer then half a second - change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 11;
          lcd.clear();
          delay(500);
        }
        }
        break; 


        case 11: // object counter
        val = digitalRead(pirPin);
        lcd.setCursor(1,0);
        lcd.print("Object Counter");
        delay(10);
        lcd.setCursor(0,1);
        lcd.print("Counted: ");
        if (val == HIGH) {
          digitalWrite(ledPin, HIGH);
          if (pirState == LOW) {
            currentState = 1;
            pirState = HIGH;
            delay(500);
          }
        }
        else {
          digitalWrite(ledPin, LOW);
          if (pirState == HIGH){
            currentState = 0;
            pirState = LOW;
          }
        }
        if(currentState != previousState){
          if(currentState == 1){
            counter = counter + 1;
            lcd.setCursor(0,1);
            lcd.print("Counted: ");
            lcd.setCursor(9,1);
            lcd.print(counter);
            delay(3000);
           }
          }
        // If button is held longer then half a second - change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 12;
          lcd.clear();
          delay(500);
        }
        break;




        case 12: // pressure unit
        lcd.setCursor(2, 0);
        lcd.print("For Pressure");
        lcd.setCursor(0, 1); // Sets the location at which subsequent text written to the LCD will be displayed
        lcd.print("Select Unit:");
        lcd.setCursor(12, 1);
        lcd.print(unitpre);
        lcd.print("  ");
        delay(10);
        if (digitalRead(selectButton) == 0) {
        if (unitPresSelect == 0) {
          unitpre = "Pa";
          unitPresSelect = 1;
        }
        else if (unitPresSelect == 1) {
          unitpre = "torr";
          unitPresSelect = 2;
        }
        else if (unitPresSelect == 2) {
          unitpre = "psi";
          unitPresSelect = 3;
        }
        else if (unitPresSelect == 3) {
          unitpre = "mbar";
          unitPresSelect = 4;
        }
        else if (unitPresSelect == 4) {
          unitpre = "inHg";
          unitPresSelect = 0;
        }
        // If button is held longer then half a second - change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 13;
          lcd.clear();
          delay(500);
        }
        }
        break;

      case 13: // get presure
        p0 = getvalue();
        P = getvalueP(); // rel. pressure
        lcd.setCursor(0, 0);
        lcd.print("Abs.:");
        lcd.setCursor(5, 0);
        lcd.print(P);
        lcd.setCursor(12, 0);
        lcd.print(unitpre);
        lcd.setCursor(0, 1);
        lcd.print("Rel.:");
        lcd.setCursor(5, 1);
        lcd.print(p0);
        lcd.setCursor(12, 1);
        lcd.print(unitpre);
        // If button is held longer then half a second - change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 14;
          lcd.clear();
          delay(500);
        }
        break;

      case 14: // altitude unit
        lcd.setCursor(2, 0);
        lcd.print("For Altitude");
        lcd.setCursor(0, 1); // Sets the location at which subsequent text written to the LCD will be displayed
        lcd.print("Select Unit: ");
        lcd.setCursor(13, 1);
        lcd.print(unitalt);
        lcd.print("  ");
        delay(10);
        if (digitalRead(selectButton) == 0) {
        if (unitAltSelect == 0) {
          unitalt = "m";
          unitAltSelect = 1;
        }
        else if (unitAltSelect == 1) {
          unitalt = "ft";
          unitAltSelect = 0;
        }
        // If button is held longer then half a second - change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 15;
          lcd.clear();
          delay(500);
        }
        }
        break;

      case 15: // get altitude of khi.
        a = getaltitude();
        A = getsensoralt();// Distance to the nearest object
        lcd.setCursor(0, 0);
        lcd.print("Google: ");
        lcd.setCursor(8, 0);
        lcd.print(a);
        lcd.setCursor(14, 0);
        lcd.print(unitalt);
        lcd.setCursor(0, 1);
        lcd.print("Sensor: ");
        lcd.setCursor(8, 1);
        lcd.print(A);
        lcd.setCursor(14, 1);
        lcd.print(unitalt);
        // If button is held longer then half a second - change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 16;
          lcd.clear();
          delay(500);
        }
        break;


       case 16: //magnetic field sensor
        lcd.setCursor(2, 0);
        lcd.print("Flux Density");//showing name
        lcd.setCursor(0, 1);//move to second line
        lcd.print("(in Gauss):");// showing units
        ADCVALUE = String((analogRead(magnet)-515)/3.76);
        ADCVALUE.toCharArray(ADCSHOW, 5);
        lcd.print(ADCSHOW);//showing the field strength value
        lcd.print("G   ");
        lcd.setCursor(0, 0);// set the cursor to column 0, line 0
        // If button is held longer then half a second - change program
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 17;
          lcd.clear();
          delay(500);
        }
        break;

        case 17:
        raw= analogRead(analogpin);
        lcd.setCursor(4,0);
        lcd.print("Ohm meter");
        if(raw) {
          buffer= raw * Vin;
          Vout= (buffer)/1024.0;
          buffer= (Vin/Vout) -1;
          R2= R1 * buffer;
          lcd.setCursor(0,1);
          lcd.print("R: ");
          lcd.setCursor(4,0);
          lcd.print(R2);
          delay(1000);
        }
        if (digitalRead(selectButton) == 0) {
          program = 18;
          lcd.clear();
          delay(500);
        }
        break;

        



  }
}





float getDistance() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // distance in cm
  
  // Converting the units
  if (unitSelect == 1) {
    distance = distance; // cm to cm
    unit = "cm";
  }
  else if (unitSelect == 2) {
    distance = distance *  0.393701; // cm to in
    unit = "in";
  }
  else if (unitSelect == 3) {
    distance = distance * 0.01; // cm to m
    unit = "m";
  }
  else if (unitSelect == 0) {
    distance = distance * 0.0328; // cm to ft
    unit = "ft";
  }
  return distance;
}


float readTempKelvin(float cel){
  return cel+273;
}


float getTemp(){
   c = dht.readTemperature();
   f = dht.readTemperature(true);
   k = readTempKelvin(c);

   if (unitTempSelect == 2){
    temp = c;
    unitTemp = "C";
   }
   else if (unitTempSelect == 1){
    temp = f;
    unitTemp = "F";
   }
   else if (unitTempSelect == 0){
    temp = k;
    unitTemp = "K";
   }
   return temp;
}



float getHumidity(){
  return Humidity = dht.readHumidity();
}



float luxmeter(){
  ldrRawData = analogRead(LDR_PIN);
  resistorVoltage = (float)ldrRawData / MAX_ADC_READING * ADC_REF_VOLTAGE;
  ldrVoltage = ADC_REF_VOLTAGE - resistorVoltage;
  ldrResistance = ldrVoltage/resistorVoltage * REF_RESISTANCE;
  ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);

  if(unitLuxSelect == 0){
    ldrLux = ldrLux;
    unitLux = "Lux";
  }
  else if(unitLuxSelect == 1){
    ldrLux = ldrLux * 0.092903;
    unitLux = "ftdc";
  }
  return ldrLux;
}

int getaltitude(){
  a = ALTITUDE;

  if (unitAltSelect == 0){
    a = a;
    unitalt = "m";
  }
  else if(unitAltSelect == 1){
    a = a*3.28084;
    unitalt = "ft";
  }  
  return a;
}

int getsensoralt(){
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    { 
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          PRESSURE = P;
        }
      }
    }
  }

  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    { 
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          p0 = pressure.sealevel(P,ALTITUDE);       
        }
      }
    }
  }
  
  A = pressure.altitude(PRESSURE,p0);
  if (unitAltSelect == 0){
    A = A;
    unitalt = "m";
  }
  else if(unitAltSelect == 1){
    A = A*3.28084;
    unitalt = "ft";
  }  
  return A;
}

float readPressureAndTemperature(){
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    { 
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          p0 = pressure.sealevel(P,ALTITUDE);       
          return p0;
        }
      }
    }
  }
}

float getvalue(){
  p0 = readPressureAndTemperature();

  if (unitPresSelect == 0){
    p0 = p0;
    unitpre = "mbar";
  }
  else if(unitPresSelect == 1){
    p0 = p0 * 0.0295333727;
    unitpre = "inHg";
  } 
  else if(unitPresSelect == 2){
    p0 = p0 * 100;
    unitpre = "Pa";
  }
  else if(unitPresSelect == 3){
    p0 = p0 * 0.750062;
    unitpre = "torr";
  }
  else if(unitPresSelect == 4){
    p0 = p0 * 0.0145038;
    unitpre = "psi";
  } 
  return round(p0); 
}

float readPressure(){
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    { 
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          PRESSURE = P;
          return P;
        }
      }
    }
  }
}
float getvalueP(){
  P = readPressure();

  if (unitPresSelect == 0){
    P = P;
    unitpre = "mbar";
  }
  else if(unitPresSelect == 1){
    P = P * 0.0295333727;
    unitpre = "inHg";
  } 
  else if(unitPresSelect == 2){
    P = P * 100;
    unitpre = "Pa";
  }
  else if(unitPresSelect == 3){
    P = P * 0.750062;
    unitpre = "torr";
  }
  else if(unitPresSelect == 4){
    P = P * 0.0145038;
    unitpre = "psi";
  } 
  return round(P); 
}
