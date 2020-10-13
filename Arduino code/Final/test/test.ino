//Library
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin define
int PinA0 = 0;
int led11 = 11; // LED11 use to learn how button work
int led12 = 12; // LED12 for extra function
int button = 8;
int pedo = 2;

/* --------------------------------------------------------*/

unsigned long waitTime = 1000; // time hold button

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

unsigned long lastChangedTime; // last time changing state of hold button

/* Temperature sensor */
float Vout = 0;
float Temp = 0;

/* LCD */
int countpedo = 0;
int counttemp = 0;

/**************************************************************************************/

// Function prototype
void printtolcd(char *s, float data);
void printinlcd(char *s, float data);
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

  Serial.begin(9600);
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
  
  PinA0 = analogRead(A0);
  Vout = PinA0 / 0.2046;
  
  Temp = (Vout - 2098.84)/-10.817; /* range of temperature -10¤C to 50¤C */
                                   /* Caculated by equation V - V1 = (V2-V1)/(T2-T1)*(T-T1)  */
  Serial.println(Temp);
}

void TemperaturetoLCD(){
  if(counttemp==0){
    printtolcd("Temp", Temp);
    
  }
  else{
    printinlcd("Temp", Temp);
  }
  counttemp++;
}


// Pedometer function for reading data from sensor
void Pedometer(){
  int PedoStatus = digitalRead(pedo);
  
  if(PedoStatus == HIGH){
     pedoCount++;
     delay(300); 
  }else{}
  Serial.println(pedoCount);
}

void PedometertoLCD(){
  if(countpedo==0){
    printtolcd("Step", pedoCount);
    
  }
  else{
    printinlcd("Step", pedoCount);
  }
  countpedo++;
}

/***********************************************************************************/

//LCD PART
void printtolcd(char *s, float data){
  lcd.begin();
  lcd.setCursor(16,0);
  lcd.print(s);
  lcd.print(" ");
  lcd.print(data);
  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
  lcd.scrollDisplayLeft();
  delay(30);
  }
}

void printinlcd(char *s, float data){
  lcd.begin();
  lcd.setCursor(0,0);
  lcd.print(s);
  lcd.print(" ");
  lcd.print(data);
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
      counttemp = 0;
    }
    else{
      TemperaturetoLCD();
      countpedo = 0;
    }
}
