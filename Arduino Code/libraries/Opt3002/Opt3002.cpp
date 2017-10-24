/*

Arduino library for Texas Instruments OPT3002 Light to Digital Sensor
Written by AA for ClosedCube
---

The MIT License (MIT)

Copyright (c) 2016 ClosedCube Limited

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
#include <Wire.h>
#include "Opt3002.h"

Opt3002::Opt3002()
{
}

OPT3002_ErrorCode Opt3002::begin(uint8_t address) {
	OPT3002_ErrorCode error = NO_ERROR;
	_address = address;
	Wire.begin();

	return NO_ERROR;
}

uint16_t Opt3002::readManufacturerID() {
	uint16_t result = 0;
	OPT3002_ErrorCode error = writeData(MANUFACTURER_ID);
	if (error == NO_ERROR)
		error = readData(&result);
	return result;
}

uint16_t Opt3002::readDeviceID() {
	uint16_t result = 0;
	OPT3002_ErrorCode error = writeData(DEVICE_ID);
	if (error == NO_ERROR)
		error = readData(&result);
	return result;
}

OPT3002_Config Opt3002::readConfig() {
	OPT3002_Config config;
	OPT3002_ErrorCode error = writeData(CONFIG);
	if (error == NO_ERROR)
		error = readData(&config.rawData);
	return config;
}

OPT3002_ErrorCode Opt3002::writeConfig(OPT3002_Config config) {
	Wire.beginTransmission(_address);
	Wire.write(CONFIG);
	Wire.write(config.rawData >> 8);
	Wire.write(config.rawData & 0x00FF);
	return (OPT3002_ErrorCode)(-10 * Wire.endTransmission());
}

OPT3002 Opt3002::readResult() {
	return readRegister(RESULT);
}

OPT3002 Opt3002::readHighLimit() {
	return readRegister(HIGH_LIMIT);
}

OPT3002 Opt3002::readLowLimit() {
	return readRegister(LOW_LIMIT);
}

OPT3002 Opt3002::readRegister(OPT3002_Commands command) {
	OPT3002_ErrorCode error = writeData(command);
	if (error == NO_ERROR) {
		OPT3002 result;
		result.lux = 0;
		result.error = NO_ERROR;

		OPT3002_ER er;
		error = readData(&er.rawData);
		if (error == NO_ERROR) {
			result.raw = er;
			result.lux = (1.2)*(pow(2, er.Exponent)*er.Result);
			//Serial.print("Exponent: "); Serial.println(er.Exponent);
			//Serial.print("Result: "); Serial.println(er.Result);
		}
		else {
			result.error = error;
		}

		return result;
	}
	else {
		return returnError(error);
	}
}
OPT3002_ErrorCode Opt3002::writeData(OPT3002_Commands command)
{
	Wire.beginTransmission(_address);
	Wire.write(command);
	return (OPT3002_ErrorCode)(-10 * Wire.endTransmission(true));
}

OPT3002_ErrorCode Opt3002::readData(uint16_t* data)
{
	uint8_t	buf[2];

	Wire.requestFrom(_address, (uint8_t)2);

	int counter = 0;
	while (Wire.available() < 2)
	{
		counter++;
		delay(10);
		if (counter > 250)
			return TIMEOUT_ERROR;
	}

	Wire.readBytes(buf, 2);
	*data = (buf[0] << 8) | buf[1];

	return NO_ERROR;
}


OPT3002 Opt3002::returnError(OPT3002_ErrorCode error) {
	OPT3002 result;
	result.lux = 0;
	result.error = error;
	return result;
}
