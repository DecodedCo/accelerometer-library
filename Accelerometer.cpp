#include "Arduino.h"
#include "Accelerometer.h"

//This is a list of some of the registers available on the ADXL345.
//To learn more about these and the rest of the registers on the ADXL345, read the datasheet!
char POWER_CTL = 0x2D;  //Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32; //X-Axis Data 0
char DATAX1 = 0x33; //X-Axis Data 1
char DATAY0 = 0x34; //Y-Axis Data 0
char DATAY1 = 0x35; //Y-Axis Data 1
char DATAZ0 = 0x36; //Z-Axis Data 0
char DATAZ1 = 0x37; //Z-Axis Data 1

Accelerometer::Accelerometer(int CS) {
    //Initiate an SPI communication instance.
  SPI.begin();
  //Configure the SPI connection for the ADXL345.
  SPI.setDataMode(SPI_MODE3);

  //Set up the Chip Select pin to be an output from the Arduino.
  pinMode(CS, OUTPUT);
  //Before communication starts, the Chip Select pin needs to be set high.
  digitalWrite(CS, HIGH);

	//Put the ADXL345 into +/- 4G range by writing the value 0x01 to the DATA_FORMAT register.
	writeRegister(DATA_FORMAT, 0x01);
	//For +/- 4G (8G range) with 10 bits, scale is calculated as 8/2^10
	scale = 0.0078125;
	//Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
	writeRegister(POWER_CTL, 0x08);  //Measurement mode  
}

String Accelerometer::processor() {
  String output;
  for (int axis=0; axis<3; axis++) {
    average_delta_a[axis]  = 0;
  }
  
  for (int i=0; i<iterations; i++) {
    readAccelerometer();
    for (int axis=0; axis<3; axis++) {
      //delta_a[axis][i]=current_acceleration[axis] - previous_acceleration[axis];
      //average_delta_a[axis] += delta_a[axis][i];
      average_delta_a[axis] += current_acceleration[axis] - previous_acceleration[axis];
    }
    delay(delta);
  }

  for (int axis=0; axis<3; axis++) {
    average_delta_a[axis] = average_delta_a[axis] / iterations;
    
    if (abs(average_delta_a[axis]) > threshold
      && (millis() - previous_event_time[axis]) > event_threshold) {
      if (previous_event[axis] > 0 && average_delta_a[axis] < 0) { // turning point
        output = axes[axis][0];
      } else if (previous_event[axis] < 0 && average_delta_a[axis] > 0) { // turning point 
        output = axes[axis][1];
      }
      previous_event_time[axis] = millis(); // update last event
      previous_event[axis] = average_delta_a[axis]; // store this last acceleration value
    }
    
  }
  
  // store previous acceleration
  for (int i=0; i<NELEMS(current_acceleration); i++) {
    previous_acceleration[i] = current_acceleration[i];
  }
  return output;
}


void Accelerometer::readAccelerometer() {
    //Reading 6 bytes of data starting at register DATAX0 will retrieve the x,y and z acceleration values from the ADXL345.
  //The results of the read operation will get stored to the values[] buffer.
  readRegister(DATAX0, 6, values);

  //The ADXL345 gives 10-bit acceleration values, but they are stored as bytes (8-bits). To get the full value, two bytes must be combined for each axis.
  //The X value is stored in values[0] and values[1].
  x = ((int)values[1]<<8)|(int)values[0];
  current_acceleration[0] = scale*x;
  //The Y value is stored in values[2] and values[3].
  y = ((int)values[3]<<8)|(int)values[2];
  current_acceleration[1] = scale*y;
  //The Z value is stored in values[4] and values[5].
  z = ((int)values[5]<<8)|(int)values[4];
  current_acceleration[2] = scale*z;
}

//This function will write a value to a register on the ADXL345.
//Parameters:
//  char registerAddress - The register to write a value to
//  char value - The value to be written to the specified register.
void Accelerometer::writeRegister(char registerAddress, char value){
  //Set Chip Select pin low to signal the beginning of an SPI packet.
  digitalWrite(CS, LOW);
  //Transfer the register address over SPI.
  SPI.transfer(registerAddress);
  //Transfer the desired register value over SPI.
  SPI.transfer(value);
  //Set the Chip Select pin high to signal the end of an SPI packet.
  digitalWrite(CS, HIGH);
}

//This function will read a certain number of registers starting from a specified address and store their values in a buffer.
//Parameters:
//  char registerAddress - The register addresse to start the read sequence from.
//  int numBytes - The number of registers that should be read.
//  char * values - A pointer to a buffer where the results of the operation should be stored.
void Accelerometer::readRegister(char registerAddress, int numBytes, char * values){
  //Since we're performing a read operation, the most significant bit of the register address should be set.
  char address = 0x80 | registerAddress;
  //If we're doing a multi-byte read, bit 6 needs to be set as well.
  if(numBytes > 1)address = address | 0x40;
  
  //Set the Chip select pin low to start an SPI packet.
  digitalWrite(CS, LOW);
  //Transfer the starting register address that needs to be read.
  SPI.transfer(address);
  //Continue to read registers until we've read the number specified, storing the results to the input buffer.
  for(int i=0; i<numBytes; i++){
    values[i] = SPI.transfer(0x00);
  }
  //Set the Chips Select pin high to end the SPI packet.
  digitalWrite(CS, HIGH);
}