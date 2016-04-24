/*
  PBMonitor v 1.0 - Interrupt driven push button monitor with simple debouncing
  Triggers "an event" (calling a call back function) on release of the button
  Reports back the duration in miliseconds thebutton has peen held down after releasing 
  so that long and short presses can be determined from the user function invoked by the button
  
  The example circuit:
   * LEDs on pins 4 and 6 to ground (+ resistors)
   * switch (normally open) from pin 3 to GND (internal pull-up configured)
   * switch (normally open) from pin 2 to Vcc with a 10K pull-down resistor

 created 22.04.2016
 by Dejan 
 */

#include "id_PBmonitor.h"

// definition of the pins the LEDs are connected to
#define LED_R 6
#define LED_G 4

// definition of the pins the push buttons are connected to 
// can be active low (using internal puluup resistor) or high (external pulldown resistor must be installed)
#define PB1 3
#define PB2 2

// An example callback function to be called when button1 is pressed
// Turns on and off one by one the two LEDs in sequence with 100ms delay
void FlashLeds(unsigned long n) 
{
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  int t=100;

  Serial.print(" Button held down for: ");
  Serial.print(n);
  Serial.print("ms  - ");
  Serial.print("Service 1 started.");
  for(int i=0; i<3; i++)
  {
    digitalWrite(LED_G, HIGH); 
    delay(t); Serial.print('.');
    digitalWrite(LED_R, HIGH); 
    delay(t); Serial.print('.');
    digitalWrite(LED_G, LOW); 
    delay(t); Serial.print('.');
    digitalWrite(LED_R, LOW); 
    delay(3*t); Serial.print('.');
  }  
  Serial.println(" service 1 ended.");
}

// An example callback function to be called when button2 is pressed
// Turns on and off both of the LEDs with 300ms delay
void BlinkLeds(unsigned long n) 
{
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  int t=300;

  Serial.print(" Button held down for: ");
  Serial.print(n);
  Serial.print("ms  - ");
  Serial.print("Service 2 started.");
  for(int i=0; i<3; i++)
  {
    digitalWrite(LED_G, HIGH); 
    digitalWrite(LED_R, HIGH); 
    delay(t); Serial.print("..");
    digitalWrite(LED_G, LOW); 
    digitalWrite(LED_R, LOW); 
    delay(t); Serial.print("..");
  }  
  Serial.println(" service 2 ended.");
}

/*
// void MonitorChange1();
int cMonitor1Change=0;
PBmonitor button1(PB2, FlashLeds, MonitorChange1, LOW, 30);
void MonitorChange1() // global user defined void f() to be used for ISR from inside the class
{
  cMonitor1Change++; // can contain other code...
  button1.Change(); // must contain this call
}

// void MonitorChange2();
int cMonitor2Change=0;
PBmonitor button2(PB1, BlinkLeds, MonitorChange2, HIGH, 100); // puldown resistor must be used
void MonitorChange2() // global user defined void f() to be used for ISR from inside the class
{
  cMonitor2Change++;
  button2.Change();
}
*/

// Define the push buttons to be monitored
PUSH_BUTTON_L(button1, PB2, FlashLeds);
//SET_PB(button1, PB2, FlashLeds, MonitorChange1, LOW, 20);
//PUSH_BUTTON_H(button2, PB1, BlinkLeds);
SET_PB(button2, PB1, BlinkLeds, MonitorChange2, HIGH, 100);

void setup() {
  // put your setup code here, to run once:

//  FlashLeds(3);
///////////////////
  Serial.begin(115200);
  delay(100);

  Serial.println("Push button from interrupt example ...");
  Serial.println("Buttons:");
  Serial.print("Button #1 @pin2");
  Serial.print(button1.Type()?"(active HIGH)":"(active  LOW)");
  Serial.println(button1.IsMonitoring()?"  monitored":"   idle");
  Serial.print("Button #2 @pin3");
  Serial.print(button2.Type()?"(active HIGH)":"(active  LOW)");
  Serial.println(button2.IsMonitoring()?"  monitored":"   idle");

  Serial.println("Start monitoring...");

// Mandatory - initiate monitoring
  button1.StartMonitoring();
  button2.StartMonitoring();

  Serial.print("Button #1 @pin2");
  Serial.print(button1.Type()?"(active HIGH)":"(active  LOW)");
  Serial.println(button1.IsMonitoring()?"  monitored":"   idle");
  Serial.print("Button #2 @pin3");
  Serial.print(button2.Type()?"(active HIGH)":"(active  LOW)");
  Serial.println(button2.IsMonitoring()?"  monitored":"   idle");

  Serial.println("Push button ready ...");

} // end setup

void loop() {
  // put your main code here, to run repeatedly:
  // Nothing really happens here - EVERYTHING is interrupt driven
  // you can put any other code to monitor other sensoer / perform other tasks

  // Just monitoring the current state od the pins 2 and 3 (push button 1 & 2 are connected to)
  // the functions will fre up on button presses and will print on the serial that will appear COM monitor
  Serial.print("digitalRead(2)=");
  Serial.print(digitalRead(2));
  Serial.print("  ");
  Serial.print("digitalRead(3)=");
  Serial.print(digitalRead(3));
  Serial.println();
  
  delay(1000);
}

