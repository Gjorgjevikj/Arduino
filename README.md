# Arduino
Arduino Libraries and Projects

Arduino Interrupt Driven Push Button Monitoring with Queue

Recently I came to a need to act on a push button that will be monitored by an external interrupt. Connecting a pushbutton on the pins that support hardware interrupts enables this (although on the original Arduino there are only 2 such pins, meaning you can have only 2 such push buttons). However, Arduino Leonardo, Mega2560 and Due do have support for hardware interrupt on more pins (see https://www.arduino.cc/en/Reference/AttachInterrupt).
I came to the idea of developing a simple class library that will automate the use of push button object configured to a (hardware interrupt supported) pin that will fire a user defined function once pressed (actually once released to be exact). So here it is, hopefully someone else can find a use of it (or make a comment). 
The library performs basic software debouncing and returns the duration the button has been held pressed. Supports active low (connecting the pin to GND) and active high (connecting the pin to Vcc) pushbuttons. 

  PBMonitorQ v 1.0 - Interrupt driven push button monitor with simple debouncing. 
  Triggers a call to a user defined call back function on release of the button. 
  Reports back the duration in milliseconds the button has been held down before releasing so that long and short presses can be determined inside the user function invoked by the button.

  Active on low and active on high push buttons are supported. If active on low (when pushed connects the pin to GND) 
  internal pullup resistor is configured (there is no need for installing a pullup resistor in the circuitry),
  BUT if active on high pushbutton is used a pulldown resistor MUST be installed in the circuitry.

  Parameters that are specified in definition of the PBMonitor object:
  - The pin to which the button is connected (must be a pin that can generate hardware interrupt requests - see table below)
  - The type of the button - active high or low (will behave strange if not defined according to the way it is connected)
  - The function to be called when the button is pressed (released to be exact)
  - The function to serve as a ISR - must be void f() and must contain a call to the PBMonitor::Change() (can be generated automatically if the macro definition for declaring a PBmonitor object is used)
  - The minimum time pressed to be registered - to avoid bouncing (any press shorter than the specified time will be ignored)  
   
Interrupts need a void function (not part of the class) to act as interrupt service routine and this must be defined outside the class as a wrapper to a member function to be called on button change state (from the interrupt). Macros to automate push button object instatiation with automatic interrupt service routine (global void function) definition are provided.  

Finally, since everything is interrupt driven and is working in the "background" (ouside the the loop()), even the user defined function to be activated when the button is pressed is executed from the interrupt. The user defined functions can last and usually have a need of enabled interrupts, so a recursive call of an interrupt from another interrupt is possible (even the same one). As a result if the button is pressed multiple times while the user defined function was executing, the consecutive button press events (of the same button) will be queued and wuill fire the user function again onece its current execution finishes. The queue size can be configured, and also the strategy of how to act when the queue is full. The possible choices for strategy are: (1) to discard all button presses after the queue is full, until a space is not eptied by dequing (after the user defined function ends) or (2) to discard the oldest unserviced requests waiting in the queue by overwriting them with the most recent ones.

 - Hardware interrupts
  Board     int.0   int.1   int.2   int.3   int.4   int.5
 Uno, Nano  2   3
 Mega2560   2   3   21    20    19    18
 Leonardo   3   2   0   1
 Due            (any pin, more info http://arduino.cc/en/Reference/AttachInterrupt)

 The included example use 2 interrupt driven push buttons at the same time.
