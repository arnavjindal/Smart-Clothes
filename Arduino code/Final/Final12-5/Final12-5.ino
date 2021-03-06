#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin define
int temperature_pin = 0;
int led11 = 11; // LED11 use to learn how button work
int led12 = 12; // LED12 for extra function
int button = 8;
int pedo = 2;

/* --------------------------------------------------------*/

unsigned long waitTime = 1000; // time hold button
unsigned long lastJsonPrintTime = 0;
boolean ledStatus = 0 ;        //LED11 off as default
boolean lastledStatus = 1;

/* Hold button */
boolean lastButton_HoldStatus = 1;
boolean buttonLongPress = 1;

/* Click button */
int buttonPushCounter = 0;
int buttonState = 0;
int lastButtonState = 0;

/* Counting step */
int pedoCount = 0;
unsigned long time_total = 50;
unsigned long lastChangedTime; // last time changing state of hold button

/* Temperature sensor */
float Vout = 0;
float Temp = 0;

/* Health parameters */
float distance_per_step = 0.5; //[meter]
float weight = 50; //[kgs]
float height = 1.53; //[meters]
unsigned int age = 25;

/* Linux serial */
char inChar;

//StaticJsonDocument<200> doc;

/**************************************************************************************/

// Function prototype

void printinlcd(char *s, float data);
void printinlcdInt(char *s, int data);
void Led_blink();
void Temperature();
void Choose_program();

/*************************************************************************************/

// Setup
void setup() {
  // put your setup code here, to run once:
  pinMode(led11, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(pedo, INPUT_PULLUP);

  digitalWrite(led11, ledStatus);

  Serial.begin(115200);
  
  Serial.println("start");
  Serial.flush();
  lcd.begin();
}

/*************************************************************************************/

//Program start
void loop() {
  
/*--------------------------------------------------------------------------*/
  // Code of button choose function
  boolean reading = digitalRead(button);
  if(reading != lastButton_HoldStatus){
    lastButton_HoldStatus = reading;
    lastChangedTime = millis();
    lastledStatus =! ledStatus; /* use for hold button */
                                /* not changing the state when hold button */
                                /* but will change the state for next hold button */
  }
  if(millis() - lastChangedTime > waitTime){
    buttonLongPress = reading;
    lastChangedTime = millis();
  }
  if(buttonLongPress == false){
    ledStatus = lastledStatus;
    buttonLongPress = true;
  }
  if(ledStatus == 1){
    Choose_program();
  }
  digitalWrite(led11, ledStatus);
  
/*-----------------------------------------*/

  if(millis() - lastJsonPrintTime > 10000){ //Print out to JSon every 5 mins
    //ExportToJson();
	  PrintAll();
    lastJsonPrintTime = millis();
  }
  // Serial print /dev/ttyMCC
  inChar = '\0';
  
  while (Serial.available()) {
    
    // get the new byte:
    inChar = (char)Serial.read();
  }

  
/*-------------------------------------------------------------------------*/
  
  // Code that run when start system
  Pedometer();  // run pedo when start
  Temperature();
}

/**************************************************************************************************/

// LED function for testing
void Led_blink(){
  digitalWrite(led12, HIGH);
  delay(50);
  digitalWrite(led12, LOW);
  delay(50);
}


// Temperature function for reading data from sensor
void Temperature(){
  int count = 0; // use for distinguish the first time print to LCD with other print to LCD
  
  temperature_pin = analogRead(A0);
  Vout = temperature_pin / 0.3272;
  Temp = (Vout - 2098.84)/-10.817; /* range of temperature -10¤C to 50¤C */
                                   /* Caculated by equation V - V1 = (V2-V1)/(T2-T1)*(T-T1)  */
  delay(50);
}

void TemperaturetoLCD(){
    printinlcd("Temp", Temp);
}


// Pedometer function for reading data from sensor
void Pedometer(){
  int PedoStatus = digitalRead(pedo); 
  if(PedoStatus == HIGH){
     pedoCount++;
     delay(300); 
  }else{}
}

void PedometertoLCD(){
    printinlcdInt("Step", pedoCount);
}

float Calculating_Distance(){
  return (pedoCount * distance_per_step);
}

float Calculating_Speed(){
  float distance_total = Calculating_Distance();
  return (distance_total / time_total);
}

float Calculating_Calories(){
  float bmi = weight/sq(height);
  float usr_speed = Calculating_Speed();
  float speed_mph = usr_speed*2.2369; //speed converting: meters per second to mph (miles per hour)
  float calories_burned;
  calories_burned = pedoCount*0.04*bmi*age*speed_mph;
  return calories_burned;
}
/***********************************************************************************/

void Choose_program(){
  buttonState = digitalRead(button);
    if(buttonState != lastButtonState){
      if(buttonState == 0){
        buttonPushCounter++;
      }
    }  
    
    lastButtonState = buttonState;
    
    if(buttonPushCounter % 2 == 0){
      PedometertoLCD();
    }
    else{
      TemperaturetoLCD();
    }
}
/***********************************************************************************/
void PrintAll(){

	Serial.print("{\"Steps\":");
	Serial.print(pedoCount);
	Serial.print(",\"Temperature\":");
	Serial.print(Temp);
	Serial.print(",\"Distance\":");
	Serial.print(Calculating_Distance());
	Serial.print(",\"Speed\":");
	Serial.print(Calculating_Speed());
	Serial.print(",\"Calories\":");
	Serial.print(Calculating_Speed());
  Serial.println("},");
}

/***********************************************************************************/

//LCD PART

void printinlcd(char *s, float data){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(s);
  lcd.print(" ");
  lcd.print(data);
  delay(50);
}

void printinlcdInt(char *s, int dataint){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(s);
  lcd.print(" ");
  lcd.print(dataint);
  delay(50);
}

/***********************************************************************************/



