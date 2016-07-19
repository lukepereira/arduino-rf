// Receive

#include <SoftwareSerial.h>
SoftwareSerial hc12Serial(2, 3); // RX, TX

int ledPin = 13;
int ledPin2 = 12;
const long BAUD_RATE = 9600;    
const char * ID = "1234";        // Unique ID for RX TX pair

int idVerify;   
int A2BStatus;
int batteryPercent;
int sleepStatus;

void setup() 
{
  hc12Serial.begin(BAUD_RATE);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  Serial.begin(4800);           // For debugging 
}

void readSerial(){
    char idRead[5];                  // ID read buffer            
    char A2B[2];                     // Anti-two-block status read
    char batteryRead[4];             // Battery read of TX
    char sleepRead[2];               // Sleep status of TX
    char garbage[100];

    hc12Serial.readBytesUntil('\0',garbage,100); // serial flush for safer first read
    hc12Serial.readBytes(idRead, 4);             // Read bytes into char arrays
    hc12Serial.readBytes(A2B, 1);          
    hc12Serial.readBytes(batteryRead, 3);
    hc12Serial.readBytes(sleepRead, 1); 
    
    idRead[4] = '\0'; A2B[1] = '\0';         // Insert null terminators for safer read  
    batteryRead[3] ='\0'; sleepRead[1] ='\0';
    
    idVerify = strncmp(ID, idRead, 4);   // returns 0 if ID is valid    
    A2BStatus = atoi(A2B);               // Convert char to int
    batteryPercent = atoi(batteryRead);
    sleepStatus = atoi(sleepRead);
    
    Serial.println("Receiving Status Update");
    Serial.print("ID Compare: ");Serial.println(idVerify);
    Serial.print("A2B: ");Serial.println(A2BStatus);
    Serial.print("Battery: ");Serial.println(batteryPercent);
    Serial.print("Sleep: ");Serial.println(sleepStatus);
    Serial.println();
}

void loop() 
{
    boolean ledState = digitalRead(ledPin);     
    if (hc12Serial.available() >= 10) {  // minimum valid serial length    
      readSerial();
      
      if (!idVerify) {  // Valid ID 
        
        if(A2BStatus == 1){
          digitalWrite(ledPin, HIGH);
        } else {
           digitalWrite(ledPin, LOW);
        }
        
        if (batteryPercent <= 25) { 
          digitalWrite(ledPin2, HIGH);  
          Serial.println("Low battery");
        } else {
          digitalWrite(ledPin2, LOW);
        }
        
        while (sleepStatus >= 2) {
          Serial.println("Transmitter is Sleeping");
          digitalWrite(ledPin, HIGH);   
          delay(1000);              
          digitalWrite(ledPin, LOW);    
          delay(1000);            
          if (hc12Serial.available() >= 10) {  // Serial Message recieved
            readSerial();
            if (!idVerify){    // Valid Serial
              break;  
            }
          }
        }
        
      }
      hc12Serial.flush();     
    } 
    delay(20);                
}
