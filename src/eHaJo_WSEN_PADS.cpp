// ***************************
// 
// Arduino library for 
// 
// WSEN_PADS-Addon-Board
//
// (C) 2019, Elektronik Hannes Jochriem, Germany
// 
// Licensed under the MIT-license, see LICENSE.txt for further information.
//
// ***************************

#include "eHaJo_WSEN_PADS.h"


// ***************************
// 
// Constructor

EHAJO_WSEN_PADS::EHAJO_WSEN_PADS()
{	// Set standard address
	_wsen_pads_address = WSEN_PADS_BASE_ADDRESS;
	
	// set shadow register with standard values
	_int_cfg.byte = 0;
	_interface_ctrl.byte = 0;
	_ctrl_1.byte = 0;
	_ctrl_2.byte = 0;
	_ctrl_3.byte = 0;
	_fifo_ctrl.byte = 0;
}

// ***************************
// 
// public stuff

bool EHAJO_WSEN_PADS::begin(WSEN_PADS_ODR opmode)
{
	// check if the chip is here
	if (read8bit(WSEN_PADS_REGISTER_DEVICE_ID)!=0xB3)
	{	// chip not present
		return false;
	}
	_ctrl_2.sw_reset = 1;
	updateRegister(WSEN_PADS_REGISTER_CTRL_2);
	while(_ctrl_2.sw_reset == 1)
	{	// wait for the reset to be finished
		readRegister(WSEN_PADS_REGISTER_CTRL_2);
	}
	
	// read all the registers just to be sure
	_int_cfg.byte = read8bit(WSEN_PADS_REGISTER_INT_CFG);
	_interface_ctrl.byte = read8bit(WSEN_PADS_REGISTER_INTERFACE_CTRL);
	_ctrl_1.byte = read8bit(WSEN_PADS_REGISTER_CTRL_1);
	_ctrl_2.byte = read8bit(WSEN_PADS_REGISTER_CTRL_2);
	_ctrl_3.byte = read8bit(WSEN_PADS_REGISTER_CTRL_3);
	_fifo_ctrl.byte = read8bit(WSEN_PADS_REGISTER_FIFO_CTRL);
	
	// set the opmode (single shot or continous)
	_ctrl_1.odr = opmode;
	updateRegister(WSEN_PADS_REGISTER_CTRL_1);
	
	switch(opmode) {
		case WSEN_PADS_SINGLE:
			// standard is power down and single conversion mode
			_wsen_pads_mode = WSEN_PADS_SINGLE;
			break;
		default:
			// continous mode with opcode-speed defined for everything else
			_wsen_pads_mode = WSEN_PADS_CONTINOUS;
			break;
	}
	return true;
}

void EHAJO_WSEN_PADS::setAddress(uint8_t sao)
{
	_wsen_pads_address |= (sao &0b00000001);
}

void EHAJO_WSEN_PADS::enableBlockdata()
{
	_ctrl_1.bdu = 1;
	updateRegister(WSEN_PADS_REGISTER_CTRL_1);
}

void EHAJO_WSEN_PADS::disableBlockdata()
{
	_ctrl_1.bdu = 0;
	updateRegister(WSEN_PADS_REGISTER_CTRL_1);
}

void EHAJO_WSEN_PADS::setLowpassfilter(WSEN_PADS_LPF filterlevel)
{
	switch(filterlevel) {
		case WSEN_PADS_ODR2:
			_ctrl_1.en_lpfp = 0;
			_ctrl_1.lpfp_cfg = 0;
			break;
		case WSEN_PADS_ODR9:
			_ctrl_1.en_lpfp = 1;
			_ctrl_1.lpfp_cfg = 0;
			break;
		case WSEN_PADS_ODR20:
			_ctrl_1.en_lpfp = 1;
			_ctrl_1.lpfp_cfg = 1;
			break;
		default: 
			_ctrl_1.en_lpfp = 0;
			_ctrl_1.lpfp_cfg = 0;
			break;
	}
	updateRegister(WSEN_PADS_REGISTER_CTRL_1);
}

void EHAJO_WSEN_PADS::enableLowNoise()
{
	uint8_t odr = _ctrl_1.odr;	// temporarely store odr value.
	_ctrl_1.odr = 0;			// set device to power down for low noise setting
	updateRegister(WSEN_PADS_REGISTER_CTRL_1);
	
	_ctrl_2.low_noise_enable = 1;
	updateRegister(WSEN_PADS_REGISTER_CTRL_2);
	
	if((_wsen_pads_mode != WSEN_PADS_SINGLE) && (odr > 4)) 
	{	// odr must be <75Hz in low noise mode.
		odr = 4;
	}
	_ctrl_1.odr = odr;
	updateRegister(WSEN_PADS_REGISTER_CTRL_1);
}
	
void EHAJO_WSEN_PADS::disableLowNoise()
{
	_ctrl_2.low_noise_enable = 0;
	updateRegister(WSEN_PADS_REGISTER_CTRL_2);
}

float EHAJO_WSEN_PADS::getPressure()
{
	uint32_t pressure;
	switch(_wsen_pads_mode) {
		case WSEN_PADS_SINGLE:
			_ctrl_2.one_shot = 1;	// start the single conversion
			updateRegister(WSEN_PADS_REGISTER_CTRL_2);
			while(_ctrl_2.one_shot == 1)	// wait for measurement to be complete
			{
				readRegister(WSEN_PADS_REGISTER_CTRL_2);
			}
		case WSEN_PADS_CONTINOUS:
			pressure = read24bit(WSEN_PADS_REGISTER_DATA_P_XL);	// get the data
			break;
		default: 
			pressure = 0;
			break;
	}
	return (float)pressure * (1.0f / 40960.0f);
}

float EHAJO_WSEN_PADS::getTemperature()
{
	uint16_t temperature;
	switch(_wsen_pads_mode) {
		case WSEN_PADS_SINGLE:
			_ctrl_2.one_shot = 1;	// start the single conversion
			updateRegister(WSEN_PADS_REGISTER_CTRL_2);
			while(_ctrl_2.one_shot == 1)	// wait for measurement to be complete
			{
				readRegister(WSEN_PADS_REGISTER_CTRL_2);
			}
		case WSEN_PADS_CONTINOUS:
			temperature = read16bit(WSEN_PADS_REGISTER_DATA_T_L);	// get the data
			break;
		default: 
			temperature = 0;
			break;
	}
	return (float)temperature * 0.01f;
}

void EHAJO_WSEN_PADS::getValues(float *pressure, float *temperature)
{
	uint32_t t_pressure;
	uint16_t t_temperature;
	uint8_t p1, p2, p3;
	switch(_wsen_pads_mode) {
		case WSEN_PADS_SINGLE:
			_ctrl_2.one_shot = 1;	// start the single conversion
			updateRegister(WSEN_PADS_REGISTER_CTRL_2);
			while(_ctrl_2.one_shot == 1)	// wait for measurement to be complete
			{
				readRegister(WSEN_PADS_REGISTER_CTRL_2);
			}
		case WSEN_PADS_CONTINOUS:
			Wire.beginTransmission(_wsen_pads_address);
			Wire.write(WSEN_PADS_REGISTER_DATA_P_XL);
			Wire.endTransmission();
			
			Wire.requestFrom(_wsen_pads_address, (uint8_t)5);
			p1 = Wire.read();
			p2 = Wire.read();
			p3 = Wire.read();
			t_temperature = Wire.read();
			t_temperature |= (Wire.read()<<8);
			t_pressure = p3;
			t_pressure <<= 8;
			t_pressure |= p2;
			t_pressure <<= 8;
			t_pressure |= p1;
			break;
		default: 
			t_pressure = 0;
			t_temperature = 0;
			break;
	}
	*pressure = (float)t_pressure * (1.0f / 40960.0f);
	*temperature = (float)t_temperature * 0.01f;
}
	

// ***************************
// 
// private stuff

uint8_t EHAJO_WSEN_PADS::read8bit(uint8_t address)
{
	Wire.beginTransmission(_wsen_pads_address);
	Wire.write(address);
	Wire.endTransmission();
	Wire.requestFrom(_wsen_pads_address, (uint8_t)1);
	return Wire.read();
}

uint16_t EHAJO_WSEN_PADS::read16bit(uint8_t address)
{
	Wire.beginTransmission(_wsen_pads_address);
	Wire.write(address);
	Wire.endTransmission();
	
	Wire.requestFrom(_wsen_pads_address, (uint8_t)2);
	uint16_t t = Wire.read();
	t |= (Wire.read()<<8);
	return t;
}

uint32_t EHAJO_WSEN_PADS::read24bit(uint8_t address)
{
	Wire.beginTransmission(_wsen_pads_address);
	Wire.write(address);
	Wire.endTransmission();
	
	Wire.requestFrom(_wsen_pads_address, (uint8_t)3);
	uint8_t t1 = Wire.read();
	uint8_t t2 = Wire.read();
	uint8_t t3 = Wire.read();
	uint32_t t = t3;
	t <<= 8;
	t |= t2;
	t <<= 8;
	t |= t1;
	return t;
}

void EHAJO_WSEN_PADS::write16bit(uint8_t address, uint16_t value)
{
	Wire.beginTransmission(_wsen_pads_address);
	Wire.write(address);
	Wire.write((uint8_t)(value & 0x00ff));
	Wire.write((uint8_t)((value & 0xff00) >> 8));
	Wire.endTransmission();
}

void EHAJO_WSEN_PADS::write8bit(uint8_t address, uint8_t value)
{
	Wire.beginTransmission(_wsen_pads_address);
	Wire.write(address);
	Wire.write(value);
	Wire.endTransmission();
}

void EHAJO_WSEN_PADS::updateRegister(uint8_t reg)
{
	switch (reg) {
		case WSEN_PADS_REGISTER_INT_CFG:
			write8bit(WSEN_PADS_REGISTER_INT_CFG, _int_cfg.byte);
			break;
		case WSEN_PADS_REGISTER_INTERFACE_CTRL:
			write8bit(WSEN_PADS_REGISTER_INTERFACE_CTRL, _interface_ctrl.byte);
			break;
		case WSEN_PADS_REGISTER_CTRL_1:
			write8bit(WSEN_PADS_REGISTER_CTRL_1, _ctrl_1.byte);
			break;
		case WSEN_PADS_REGISTER_CTRL_2:
			write8bit(WSEN_PADS_REGISTER_CTRL_2, _ctrl_2.byte);
			break;
		case WSEN_PADS_REGISTER_CTRL_3:
			write8bit(WSEN_PADS_REGISTER_CTRL_3, _ctrl_3.byte);
			break;
		case WSEN_PADS_REGISTER_FIFO_CTRL:
			write8bit(WSEN_PADS_REGISTER_FIFO_CTRL, _fifo_ctrl.byte);
			break;
		default: // do nothing
			break;
	}
}

void EHAJO_WSEN_PADS::readRegister(uint8_t reg)
{
	switch (reg) {
		case WSEN_PADS_REGISTER_INT_CFG:
			_int_cfg.byte = read8bit(WSEN_PADS_REGISTER_INT_CFG);
			break;
		case WSEN_PADS_REGISTER_INTERFACE_CTRL:
			_interface_ctrl.byte = read8bit(WSEN_PADS_REGISTER_INTERFACE_CTRL);
			break;
		case WSEN_PADS_REGISTER_CTRL_1:
			_ctrl_1.byte = read8bit(WSEN_PADS_REGISTER_CTRL_1);
			break;
		case WSEN_PADS_REGISTER_CTRL_2:
			_ctrl_2.byte = read8bit(WSEN_PADS_REGISTER_CTRL_2);
			break;
		case WSEN_PADS_REGISTER_CTRL_3:
			_ctrl_3.byte = read8bit(WSEN_PADS_REGISTER_CTRL_3);
			break;
		case WSEN_PADS_REGISTER_FIFO_CTRL:
			_fifo_ctrl.byte = read8bit(WSEN_PADS_REGISTER_FIFO_CTRL);
			break;
		default: // do nothing
			break;
	}
}