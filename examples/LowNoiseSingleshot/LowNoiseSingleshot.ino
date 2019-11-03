// ***************************
// 
// Get pressure from WSEN-PADS-addon
// Use low-noise mode and lowpassfilter 2 with max filtering
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
	sensor.enableLowNoise();	// enable low noise mode
	sensor.setLowpassfilter(WSEN_PADS_ODR20);	// set lpf2 to max filtering
}

void loop() {
	Serial.print("Pressure = ");
	Serial.print(sensor.getPressure() * 10.0);	// convert to mbar
	Serial.println(" mbar");

	delay(200);
}