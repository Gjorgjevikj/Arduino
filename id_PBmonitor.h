/*
  PBMonitor v 1.0 - Interrupt driven push button monitor with simple debouncing
  Triggers a call to a user defined call back function on release of the button
  Reports back the duration in milliseconds the button has been held down before releasing 
  so that long and short presses can be determined inside the user function invoked by the button

  Active on low and active on high push buttons are supported. If active on low (when pushed connects the pin to GND) 
  internal pullup resistor is configured, so there is no need for installing a pullup resistor in the circuitry
  BUT if active on high pushbutton is used a pulldown resistor MUST be installed in the circuitry

  Parameters that are specified in definition of the PBMonitor object:
  - The pin to which the button is connected (must be a pin that can generate hardware interrupt requests - see table below)
  - The type of the button - active high or low (will behave strange if not defined according to the way it is connected)
  - The function to be called when the button is pressed (released to be exact)
  - The function to serve as a ISR - must be void f() and must contain a call to the Change() member function of that 
    PBMonitor class for the object (can be generated automatically if the macro definition for declaring a PBmonitor is used)
  - The minimum time pressed to be registered - to avoid bouncing (a press shorter than the specified time will be ignored) 
  
 created 22.04.2016
 by Dejan Gjorgjevikj
 
 - Hardware interrupts
  Board     int.0   int.1   int.2   int.3   int.4   int.5
 Uno, Nano  2   3
 Mega2560   2   3   21    20    19    18
 Leonardo   3   2   0   1
 Due            (any pin, more info http://arduino.cc/en/Reference/AttachInterrupt)

 This example use 2 interrupt driven push buttons at the same time
 */ 

#define Q_SIZE 4
#include "OverwriteQueue.h"

// Macros to automate Push button object instatiation with interrupt service routine global function definition 
// Interrupts needs a void function (not part of the class) to act as interrupt service routine that must be defined outside the class 
// as a wrapper to member function to be called on button change state (from the interrupt)
#define SET_PB(PBUTON, PIN_BUT, FP_CALLB, FP_ISR, TYPE, T_IGNORE) PBmonitor PBUTON(PIN_BUT, FP_CALLB, PBUTON##_ISR, TYPE, T_IGNORE); \
void PBUTON##_ISR() { PBUTON.Change(); }
#define PUSH_BUTTON_L(PBUTON, PIN_BUT, FP_CALLB) PBmonitor PBUTON(PIN_BUT, FP_CALLB, PBUTON##_ISR, LOW); \
void PBUTON##_ISR() { PBUTON.Change(); }
#define PUSH_BUTTON_H(PBUTON, PIN_BUT, FP_CALLB) PBmonitor PBUTON(PIN_BUT, FP_CALLB, PBUTON##_ISR, HIGH); \
void PBUTON##_ISR() { PBUTON.Change(); }

typedef void (*ISR)(); // pointer to void function (to act as interrupt service routine)
typedef void (*PBcallback) (unsigned long); // pointer to void function taking one int (should be unsigned long) 
// to be called from the push button monitor object when the button wil be released passing the number od miliseconds the button was held down

class PBmonitor // the class represntin a push button to be monitored using interrupts
{
  public:
    PBmonitor(int p, PBcallback f, ISR isrv, bool act = HIGH, unsigned long t_i = 20) : 
      pin(p), isr(isrv), callback(f), active(act), t_ignore(t_i), e_millis(0), 
      inCallback(false), pstate(act?LOW:HIGH), monitoring(false)//, toCallService(0)
      {  }
    ~PBmonitor() { StopMonitoring(); }
    void StartMonitoring() 
    { 
      digitalWrite(pin, active==LOW ? HIGH : LOW); 
      pinMode(pin, active==LOW ? INPUT_PULLUP : INPUT); // internal pull-up resistor or external pulldown
      pstate = digitalRead(pin);
      monitoring=true;
      attachInterrupt(digitalPinToInterrupt(pin), isr, CHANGE); // set interrupt on change
    }
    void StopMonitoring() 
    { 
      detachInterrupt(digitalPinToInterrupt(pin)); 
      monitoring=false; 
    }
    bool IsMonitoring() const { return monitoring; } // is monitored = ISR installed
    bool Type() const { return active; } // active high or low
    int Pending() const { return pushes.waiting(); } //toCallService; }
    
    void Change() 
    { 
      bool push_registered=false;
      uint8_t oldSREG = SREG; // Save the status
      interrupts();
      unsigned long now=millis();
      SREG = oldSREG;
      bool state = digitalRead(pin);
      if(pstate == HIGH && state == LOW)
      {
        if(active == HIGH)
          push_registered=true;
        else
          e_millis = now; // just store the time when pushed down
      }
      else if(pstate == LOW && state == HIGH)
      { // released - take action now
        if(active == HIGH)
          e_millis = now; // just store the time when pushed down
        else
          push_registered=true;
      }
      pstate=state;
      unsigned long duration=now-e_millis;
      if(push_registered && duration > t_ignore) // was pressed long enought
      {
        if(pushes.enqueue(duration))
        {
          if(!inCallback) 
          {
            while( pushes.waiting() )
            {
              inCallback=true;
              interrupts();
              callback(pushes.dequeue());
              noInterrupts();
              inCallback=false;
            }
          }
        }
      }
    }

  private:
    bool monitoring;
    int pin; // pin at which change of level is monitored
    PBcallback callback; // pointer to function to be called when button is pressed
    ISR isr; // pointer to void f() function to serve as interrupt service routine - must be defined on a global scope
    bool active; // type of switch active on HIGH or LOW, when set to be active on HIGH, pulldown resistor must be connected
    volatile bool inCallback; // true while executing the callback function, will not re-enter the function from the same interrupt (isr disabled)
    volatile bool pstate; // previous state of the pin (checked in the ISR)
    volatile unsigned long e_millis; // elapsed millis since the last call to ISR
    unsigned long t_ignore; // time to be ignorred - changes that appear @ t < t_ignore will be ignored
    Queue<unsigned long, Q_SIZE, true> pushes;
};


