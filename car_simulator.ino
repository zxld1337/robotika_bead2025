#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 myRFID(SS_PIN, RST_PIN); // Create MFRC522 instance.

int pinLED_RED=7;
int pinLED_GREEN=8;

bool carLocked = true;
String validKeyHash = "50 22 DE 30";  

const int echoPin = 4;
const int trigPin = 3;
const int buzzerPower = 6;

long duration;
int distance;
int safetyDistance;

 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();          // Initiate  SPI bus
  myRFID.PCD_Init();    // Initiate MFRC522

  Serial.println(F("\nPlease scan your RFID card..."));  
  
  pinMode(pinLED_RED, OUTPUT);
  pinMode(pinLED_GREEN, OUTPUT);

  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(buzzerPower, OUTPUT);

  digitalWrite(buzzerPower, LOW);
  digitalWrite(pinLED_RED, HIGH); // init locked car (red led) 
}


void loop() 
{
  // Wait for RFID card
  if (!myRFID.PICC_IsNewCardPresent() || !myRFID.PICC_ReadCardSerial()) {
    if (!carLocked) {      
      initCar();
    }
    return;
  }

  Serial.print(F("\nUSER ID tag:"));
  String content= "";
  for (byte i = 0; i < myRFID.uid.size; i++) 
  {
     Serial.print(myRFID.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(myRFID.uid.uidByte[i], HEX);
     content.concat(String(myRFID.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(myRFID.uid.uidByte[i], HEX));
  }
  delay(500);

  content.toUpperCase();
  if (content.substring(1) != validKeyHash) //change here the UID of the card
  {
    if (carLocked)
    {
     Serial.println(F("\nFeloldás elutasítva!")) ;
     return;
    }
    Serial.println(F("\nLezárás elutasítva!"));
    return;   
  }

  // Toggle lock state
  carLocked ? unlockCar() : lockCar();
}


void unlockCar() {   
  digitalWrite(pinLED_RED, LOW);
  digitalWrite(pinLED_GREEN, HIGH);
  carLocked = false;
  
  Serial.println(F("\nAz autó feloldva!"));
}

void lockCar() {    
  digitalWrite(pinLED_RED, HIGH);
  digitalWrite(pinLED_GREEN, LOW);
  digitalWrite(buzzerPower, LOW);
  carLocked = true;;
  
  Serial.println(F("\nAz autó lezárva!"));
}


void initCar() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.034) / 2; 

  if (distance <= 5){
    digitalWrite(buzzerPower, HIGH);
    Serial.print("Ne tolasson tovább! Már csak: ");
    Serial.print(distance);
    Serial.println("0 cm van az autó mögött!");
  } else {
    digitalWrite(buzzerPower, LOW);
  }

}
