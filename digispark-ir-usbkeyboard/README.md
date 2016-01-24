# digispark-ir-usbkeyboard

Receives RC5 infrared remote control codes and simulates an USB keyboard that sends hid-generic input,hidraw "USB HID v1.01 Keyboard" to the host computer.

## Hardware

 * Digispark (ATtiny board with USB connector using V-USB) ([Digistump](http://digistump.com/products/1) or [eBay](http://www.ebay.com/itm/Digispark-Kickstarter-Micro-General-USB-Development-Board-For-Arduino-ATTINY85-W-/221558382938?hash=item3395e8a15a:g:DPoAAOSw7NNUI8kI))
 * Infrared receiver, e.g., TSOP4838 ([eBay](http://www.ebay.com/itm/10PCS-38-kHz-Remote-TSOP4838-DIP-3-IR-Receiver-Infrared-Radiation-Module-/311525671902?hash=item48886083de:g:-fsAAOSwaNBUcF-l))
 
```
                  |-----|
                  | ( ) |
                  |-----|
                  |  |  |
                  |  |  |
Digispark Pins 0  1  2 
0 = Data In
1 = GND (pin pulled low)
2 = VCC (pin pulled high)
```

## TODO

 * Receiving code sometimes misses the meaning of signals. Replace with IRLib receiving code (blocked by https://github.com/digistump/DigistumpArduino/issues/21)
 * Send something over USB to the host PC that can be recognized by LIRC, possibly along the lines of http://notepad.xavierdetourbet.com/?p=258
