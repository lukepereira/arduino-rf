# ArduinoRadioFrequency
Secure and robust radio frequency communincation between two Arduino Nano boards used for general detection of signal or malfuctions. 

Currently being applied to detect Anti-two-blocking malfunction in hydraulic lift system.

Transmitter:
- Wake from powered down sleep on Boom movement 
- Use timer to send serial string updates
- Send serial update immediately when A2B pin high
- Powered down sleep after 20 mins of inactivity 
- Level Interrupt wakes when boom movement sensor high

Reciever:
- Power-ON/Wake up with push-button 
- Verify update string, otherwise ignore serial
- Decode serial message or print lost connection warning if no message recieved in time interval
- LED1 On -> A2B Detection, LED2 On-> Transmitter battery < 25%
