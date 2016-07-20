# WirelessA2b
Wireless Anti-two-block detection with HC-12 and Arduino Nano


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
