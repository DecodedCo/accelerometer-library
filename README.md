# README

Simple library to use an accelerometer with an arduino. Make sure the library is in your sketchbook library directory and then restart Arduino

Sample program would be:

```
#include <SPI.h>
#include <Accelerometer.h>
Accelerometer a(10); //CS pin passed to Constructor
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("ready to go!");
}

void loop() {
  // processor returns a string of last movement recorded
  String output = a.processor();
  Serial.println(output);
}
```