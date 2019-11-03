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
// Pressure given in kPa
//
// ***************************

#ifndef EHAJO_WSEN_PADS_H
#define EHAJO_WSEN_PADS_H

#include <Arduino.h>
#include <Wire.h>

#define WSEN_PADS_BASE_ADDRESS 0x5C		// Base address 0b 1 0 1 1 1 0 SAO

#define WSEN_PADS_REGISTER_INT_CFG			0x0B	// Interrupt configuration register
#define WSEN_PADS_REGISTER_THR_P_L			0x0C	// Pressure threshold register L
#define WSEN_PADS_REGISTER_THR_P_H			0x0D	// Pressure threshold register H
#define WSEN_PADS_REGISTER_INTERFACE_CTRL	0x0E	// Interface control register
#define WSEN_PADS_REGISTER_DEVICE_ID		0x0F	// Device ID register 
#define WSEN_PADS_REGISTER_CTRL_1			0x10	// Control register 1
#define WSEN_PADS_REGISTER_CTRL_2			0x11	// Control register 2
#define WSEN_PADS_REGISTER_CTRL_3			0x12	// Control register 3
#define WSEN_PADS_REGISTER_FIFO_CTRL		0x13	// FIFO control register
#define WSEN_PADS_REGISTER_FIFO_WTM			0x14	// FIFO threshold register
#define WSEN_PADS_REGISTER_REF_P_L			0x15	// Reference pressure register L
#define WSEN_PADS_REGISTER_REF_P_H			0x16	// Reference pressure register H
#define WSEN_PADS_REGISTER_OPC_P_L			0x18	// Reference pressure register L
#define WSEN_PADS_REGISTER_OPC_P_H			0x19	// Reference pressure register H
#define WSEN_PADS_REGISTER_INT_SOURCE		0x24	// Interrupt register
#define WSEN_PADS_REGISTER_FIFO_STATUS_1	0x25	// FIFO status register
#define WSEN_PADS_REGISTER_FIFO_STATUS_2	0x26	// FIFO status register
#define WSEN_PADS_REGISTER_STATUS			0x27	// Status register
#define WSEN_PADS_REGISTER_DATA_P_XL		0x28	// Pressure output XLowbyte
#define WSEN_PADS_REGISTER_DATA_P_L			0x29	// Pressure output Lowbyte
#define WSEN_PADS_REGISTER_DATA_P_H			0x2A	// Pressure output Highbyte
#define WSEN_PADS_REGISTER_DATA_T_L			0x2B	// Temperature output register Lowbyte
#define WSEN_PADS_REGISTER_DATA_T_H			0x2C	// Temperature output register Highbyte
#define WSEN_PADS_REGISTER_FIFO_DATA_P_XL	0x78	// FIFO Pressure output XLowbyte
#define WSEN_PADS_REGISTER_FIFO_DATA_P_L	0x79	// FIFO Pressure output Lowbyte
#define WSEN_PADS_REGISTER_FIFO_DATA_P_H	0x7A	// FIFO Pressure output Highbyte
#define WSEN_PADS_REGISTER_FIFO_DATA_T_L	0x7B	// FIFO Temperature output register Lowbyte
#define WSEN_PADS_REGISTER_FIFO_DATA_T_H	0x7C	// FIFO Temperature output register Highbyte

enum WSEN_PADS_MODE : uint8_t {
	WSEN_PADS_SINGLE = 0,
	WSEN_PADS_CONTINOUS = 1
};

enum WSEN_PADS_ODR : uint8_t {
	WSEN_PADS_ONESHOT = 0,
	WSEN_PADS_1HZ = 1,
	WSEN_PADS_10HZ = 2,
	WSEN_PADS_25HZ = 3,
	WSEN_PADS_50HZ = 4,
	WSEN_PADS_75HZ = 5,
	WSEN_PADS_100HZ = 6,
	WSEN_PADS_200HZ = 7
};

enum WSEN_PADS_LPF : uint8_t {
	WSEN_PADS_ODR2 = 0,		// device bandwidth ODR/2,  LPF2 disabled
	WSEN_PADS_ODR9 = 1,		// device bandwidth ODR/9,  LPF2 enabled
	WSEN_PADS_ODR20 = 2		// device bandwidth ODR/20, LPF2 enabled
};


typedef union {
	struct {
		uint8_t phe:1,			// 0 INT inactive, 1 INT on pressure high event
		ple:1,					// 0 INT inactive, 1 INT on pressure low event
		lir:1,					// 0 interrupt not latched, 1 INT latched
		diff_en:1,				// 0 differential input mode disabled, 1 enabled
		reset_az:1,				// 0 normal mode, 1 reset autozero
		auto_zero:1,			// 0 autozero disabled, 1 enabled
		reset_arp:1,			// 0 normal mode, 1 reset autorefp
		auto_refp:1;			// 0 autorefp disable, 1 enable
	};
	uint8_t byte;
} int_cfg_register;

typedef union {
	struct {
		uint8_t i2c_disable:1,	// 0 i2c enabled, 1 i2c disabled
		unused_1:1,
		pd_dis_int:1,			// 0 INT pin pulldown connected, 1 no pullup
		sao_pu_en:1,			// 0 no pullup; 1 pullup on SAO
		sda_pu_en:1,			// 0 no pullup; 1 pullup on SDA
		unused:3;
	};
	uint8_t byte;
} interface_ctrl;

typedef union {
	struct {
		uint8_t unused_2:1,
		bdu:1,					// 0 data register updates continously, 1 just after MSB and LSB read
		lpfp_cfg:1,				// configure lp-filter as:
		// en_lpfp	lpfp_cfg	LPF2 status		device bandwith		samples discarded
		// 0		X			disabled		ODR/2				0
		// 1		0			enabled			ODR/9				2
		// 1		1			enabled			ODR/20				2
		en_lpfp:1,				// 0 low-pass-filter disabled, 1 enabled
		odr:3,					// selection of operating mode as:
		// 000 power-down/single shot mode
		// 001 1Hz
		// 010 10Hz
		// 011 25
		// 100 50
		// 101 75
		// 110 100
		// 111 200
		unused_1:1;
	};
	uint8_t byte;
} ctrl_1;

typedef union {
	struct {
		uint8_t one_shot:1,		// 0 normal operation, 1 a new data set is aquired
		low_noise_enable:1,		// 0 low-power mode, 1 low-noise mode
		sw_reset:1,				// 0 normal operation, 1 software reset
		unused:1,
		if_add_inc:1,			// 0 auto increment addresspointer disabled, 1 enabled (default)
		pp_od:1,				// 0 INT-output push-pull, 1 open-drain
		int_h_l:1,				// 0 INT active high, 1 active low
		boot:1;	// 0 normal operation, 1 reboot memory content
	};
	uint8_t byte;
} ctrl_2;

typedef union {
	struct {
		uint8_t int_s:2,		// interrupt event control as:
		// int_s[1]	int_s[0]	INT-pin configuration
		// 0		0			Data signal (order: DRDY | INT_F_WTM | INT_F_OVER | INT_F_FULL)
		// 0		1			pressure high event
		// 1		0			pressure low event
		// 1		1			pressure low or high event
		drdy:1,					// 0 disabled, 1 data-ready routet to INT pin
		int_f_ovr:1,			// 0 disabled, 1 FIFO overrun INT enabled
		int_f_wtm:1,			// 0 disabled, 1 FIFO threshold level INT enabled
		int_f_full:1,			// 0 disabled, 1 FIFO full INT enabled
		unused_1:2;
	};
	uint8_t byte;
} ctrl_3;

typedef union{
	struct {
		uint8_t f_mode:2,		// select fifo mode, see below
		// trig_modes	f_mode[1]	f_mode[0]	fifo mode
		// X			0			0			bypass mode
		// 0			0			1			FIFO mode
		// 0			1			X			Continuous mode
		// 1			0			1			Bypass-to-FIFO mode
		// 1			1			0			Bypass-to-Continuous mode
		// 1			1			1			Continuous-to-FIFO mode
		trig_modes:1,			// triggered fifo mode, see below
		stop_on_wtm:1,			// 0 disabled, 1 user defined fifo threshold enabled
		unused:4;
	};
	uint8_t byte;
} fifo_ctrl;



class EHAJO_WSEN_PADS
{
	// private stuff
	uint8_t _wsen_pads_address;
	WSEN_PADS_MODE _wsen_pads_mode;
	
	// i2c-read/write functions
	uint8_t read8bit(const uint8_t address);
	uint16_t read16bit(const uint8_t address);
	uint32_t read24bit(const uint8_t address);
	void write16bit(const uint8_t address, const uint16_t value);
	void write8bit(const uint8_t address, const uint8_t value);
	void updateRegister(uint8_t reg);
	void readRegister(uint8_t reg);
	
	// shadow register
	int_cfg_register _int_cfg;
	interface_ctrl _interface_ctrl;
	ctrl_1 _ctrl_1;
	ctrl_2 _ctrl_2;
	ctrl_3 _ctrl_3;
	fifo_ctrl _fifo_ctrl;
	
public:
	// constructor
	EHAJO_WSEN_PADS();
	
	// public stuff
	
	// I2C
	void setAddress(uint8_t sao);
	bool begin(WSEN_PADS_ODR opmode);
	
	// device settings
	void enableBlockdata();								// enables the blockdata mode, data just updated after registers are read
	void disableBlockdata();							// disables blockdata
	void setLowpassfilter(WSEN_PADS_LPF filterlevel);	// configure lowpassfilter as WSEN_PADS_ODR2 (standard), WSEN_PADS_ODR9 or WSEN_PADS_ODR20
	void enableLowNoise();								// enables low-noise mode for better measurement
	void disableLowNoise();

	// sensor values
	float getPressure();
	float getTemperature();
	void getValues(float *pressure, float *temperature);

};

#endif // EHAJO_WSEN_PADS_H
