# digispark-ir-usbkeyboard

Receives RC5 infrared remote control codes and simulates an USB keyboard that emits hid-generic input,hidraw "USB HID v1.01 Keyboard" signals.

## Hardware

 * Digispark (ATtiny board with USB connector using V-USB)
 * Infrared receiver, e.g., TSOP4838
 
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
