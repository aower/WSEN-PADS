// ***************************
// 
// Get temperature and pressure from WSEN-PADS-addon
// 
// WSEN-PADS-Addon-Board
//
// (C) 2019, Elektronik Hannes Jochriem, Germany
// 
// Licensed under the MIT-license, see LICENSE.txt for further information.
//
// ***************************

#include <eHaJo_WSEN_PADS.h>

EHAJO_WSEN_PADS sensor;

void setup() {
	while(!Serial) {}
	Serial.begin(9600);
	Wire.begin();	// start I2C-stuff
	Serial.println("eHaJo WSEN-PADS Addon test");
	sensor.setAddress(1); // SAO standard 1 for power safe
	if(sensor.begin(WSEN_PADS_ONESHOT))	// begin-routine for single-shot mode
	{
		Serial.println("Sensor board found");
	}
	else
	{
		Serial.println("ERROR: Sensor board not found!");
		while(1){};
	}
}

void loop() {
	float pressure;
	float temperature;
	sensor.getValues(&pressure, &temperature); // pressure in kPa, temp in Celsius
	Serial.print("Temperature = ");
	Serial.print(temperature);
	Serial.print(" C | ");
	Serial.print("Pressure = ");
	Serial.print(pressure * 10.0);	// convert to mbar
	Serial.println(" mbar");

	delay(200);
}