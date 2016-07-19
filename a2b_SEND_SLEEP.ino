// Transmit

#include <SoftwareSerial.h>
#include <LowPower.h>             // https://github.com/rocketscream/Low-Power
#include <Timer.h>                // https://github.com/JChristensen/Timer
#include <math.h>

SoftwareSerial hc12Serial(2, 4);  // RX, TX
const long BAUD_RATE = 9600;      
const int ID = 0000;              // Unique ID for RX TX pair
const int wakeUpPin = 3;          // Sleep interrupt button
const int a2bPin = 5;             // A2B detect

int inactiveTimer = 0;            // Counts boom inactivity every minute
int a2bState;
boolean lastA2bState = 0;
Timer t;

void wakeUp()
{
    // Handler for the pin interrupt.
}

void setup()
{
    pinMode(wakeUpPin, INPUT);        // Configure wake up pin as input
    pinMode(a2bPin, INPUT);
    //pinMode(6, );
    hc12Serial.begin(9600); 
    Serial.begin(4800);               // For debugging
    t.every(60000, sendTimedUpdate);  // Send update every minute
    sendSerialUpdate();               // First Update
}

void sendSerialUpdate() 
{
    float volt = (float)readVcc()/1000;
    int percent = roundf(((volt-3)/0.7)*100);   // Calculate battery percent from voltage    
    
    Serial.println("Sending serial update:");
    Serial.print("ID: ");Serial.println(ID);
    Serial.print("A2B: "); Serial.println(a2bState);
    Serial.print("Battery: "); Serial.println(percent);
    Serial.print("Sleep: "); Serial.println(inactiveTimer);
    Serial.println();
    
    hc12Serial.print('\0');   // Wrap update string in a null terminator delimiter
    hc12Serial.print(ID);
    hc12Serial.print(a2bState);
    hc12Serial.print(percent);
    hc12Serial.print(inactiveTimer);
}

void sendTimedUpdate() 
{
   Serial.print("(Timer) ");
   ++inactiveTimer;
   sendSerialUpdate();
}

void loop() 
{
    t.update();
    a2bState = digitalRead(a2bPin);
    if (a2bState != lastA2bState) {
     sendSerialUpdate();
     lastA2bState = !lastA2bState;
    }

    int boomState = digitalRead(wakeUpPin);   // Detect boom movement and reset counter 
    if (boomState == 1) {                  
      inactiveTimer = 0;
    }
    
    if (inactiveTimer >= 2) {    // 20 minutes of boom inactivity
      Serial.println("Entering Interrrupt Sleep");
      delay(175);
      
      //digitalWrite(6, LOW);
      //delay(100);
      //hc12Serial.print("AT+SLEEP");
      attachInterrupt(1, wakeUp, INPUT_PULLUP);            // Interrupt on pin 3
      LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); // Enter sleep until interrupt
      
      detachInterrupt(1);       // Disable interrupt pin 3  
      Serial.println("Waking");
      //digitalWrite(6, HIGH);
      inactiveTimer = 0;
      sendSerialUpdate();       // Notify RX of wake state 
    }
    delay(20); 
}

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2);                        // Wait for Vref to settle
  ADCSRA |= _BV(ADSC);             // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL;             // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH;             // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result;      // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result;                   // Vcc in millivolts
}
