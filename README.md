# eHaJo_WSEN-PADS

This library is fitted for the WSEN-PADS-Addon-Board from eHaJo

Please see examples for further usage of it.

# License

(C) 2019 , Elektronik Hannes Jochriem, Germany

This library is open source and released under *[MIT license](LICENSE.txt)*. Please see this file for further information.

# WSEN-PADS

The WSEN-PADS is a MEMS-based chip with 24bit resolution for the absolute pressure. It includes some cool features:
 - samplerate up to 200Hz
 - 128-stage FIFO
 - temperature sensor
 - configurable interrupt
The standard is to tie SAO high (for better energy savings), so the standard address is 0x5D:
Binary: 1 0 1 1 1 0 SAO
Hex: 0x5C and 0x5D

# Registers and Power-up condition

| Register | Name           | Default |
|----------|----------------|---------|
| 0x0B     | INT_CFG        | 0x00    |
| 0x0C     | THR_P_L        | 0x00    |
| 0x0D     | THR_P_H        | 0x00    |
| 0x0E     | INTERFACE_CTRL | 0x00    |
| 0x10     | CTRL_1         | 0x00    |
| 0x11     | CTRL_2         | 0x10    |
| 0x12     | CTRL_3         | 0x00    |
| 0x13     | FIFO_CTRL      | 0x00    |
| 0x14     | FiFO_WTM       | 0x00    |
| 0x15     | REF_P_L        | 0x00    |
| 0x16     | REF_P_H        | 0x00    |
| 0x18     | OPC_P_L        | 0x00    |
| 0x19     | OPC_P_H        | 0x00    |

# Functions in the library

## void setAddress(uint8_t sao)

First function to be called before begin() if address is set to 0x5D like in standard-board-configuration (sao=1)

## bool begin(WSEN_PADS_ODR opmode)

Set all private registers and start the sensor chip like defined in opmode.
Available opmodes are:

| opmode            | function                          |
|-------------------|-----------------------------------|
| WSEN_PADS_ONESHOT | shutdown mode / single-shot       |
| WSEN_PADS_1HZ     | continous mode, 1Hz sample rate   |
| WSEN_PADS_10HZ    | continous mode, 10Hz sample rate  |
| WSEN_PADS_25HZ    | continous mode, 25Hz sample rate  |
| WSEN_PADS_50HZ    | continous mode, 50Hz sample rate  |
| WSEN_PADS_75HZ    | continous mode, 75Hz sample rate  |
| WSEN_PADS_100HZ   | continous mode, 100Hz sample rate |
| WSEN_PADS_200HZ   | continous mode, 200Hz sample rate |

## void enableBlockdata(), void disableBlockdata()

Enable blockdata mode. 
This feature should be enabled when the reading of the data is slower than the output data
rate (ODR). By default, the BDU bit is set to ’0’ and data registers are updated continuously.
When BDU feature is enabled, reading of the pressure or temperature values sampled at
different times can be avoided.

## void setLowpassfilter(WSEN_PADS_LPF filterlevel)

The sensor embeds two digital low-pass filters. First low-pass filter LPF1 is always applied
to the pressure and temperature data. The second low-pass filter LPF2 can be optionally
enabled and applied to the pressure data. This configuration is available for both continuous
mode and single conversion mode.
The lowpass-filter modes are:

| lpf-mode        | description                           |
|-----------------|---------------------------------------|
| WSEN_PADS_ODR2  | device bandwidth ODR/2, LPF2 disabled |
| WSEN_PADS_ODR9  | device bandwidth ODR/9,  LPF2 enabled |
| WSEN_PADS_ODR20 | device bandwidth ODR/20, LPF2 enabled |

## void enableLowNoise(), void disableLowNoise()

In the low-power configuration, the device is configured to minimize the current consumption.
In the low-noise configuration, the device is configured to reduce the noise.

!Attention! The low-noise mode reduces maximum odr-speed. If you have configured the speed >75Hz, it will be cut down to 75Hz.

## 	float getPressure()

Returns the actual pressure in float and kPa with 24 bit resolution. In one-shot-mode this function triggers the conversion and waits for the result to be ready.
In continous-mode the data is just read out from the pressure-registers.

## float getTemperature()

Returns the actual temperature in float and Celsius with 16 bit resolution. Works like getPressure();

##	void getValues(float *pressure, float *temperature)

This function reads pressure and temperature in one read-operation due to auto-increment of the address-pointer.
You can save some flash with this function.
The values are written in kPa and Celsius as float into the pointer addresses.

# TODO

 - Interrupt-stuff
 - FIFO-stuff
 - some more configuration register-stuff