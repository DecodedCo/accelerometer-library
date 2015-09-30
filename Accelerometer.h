#ifndef Accelerometer_h
#define Accelerometer_h

#include "Arduino.h"
#include <SPI.h>

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))


class Accelerometer {
	public:
	Accelerometer(int CS);
	String processor();
	
	private:
	void readAccelerometer();
	void writeRegister(char registerAddress, char value);
	void readRegister(char registerAddress, int numBytes, char * values);
	//CS pin
	int CS;
	//This buffer will hold values read from the ADXL345 registers.
	char values[10];
	//These variables will be used to hold the x,y and z axis accelerometer values.
	int x,y,z;
	double scale;
	double current_acceleration[3];
	int j, delta = 10; // ms
	int iterations = 10; // to calculate delta a
	String axes[3][2] = {
	  {"Forward", "Backward"},
	  {"Left", "Right"},
	  {"Down", "Up"}
	};
	//double delta_a[3][20];
	double average_delta_a[3];
	double threshold = 0.2; // significant delta a
	unsigned long previous_event_time[3] = {0,0,0}; // when it happened
	unsigned long event_threshold = 250; // in ms, to count as movement
	double previous_event[3]; // the delta value
	double previous_acceleration[3];
};

#endif