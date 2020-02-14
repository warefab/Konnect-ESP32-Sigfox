/*
   LIS2DH12.c

   lis2dh12 3-axis sensor functions
   Warefab Konnect STM32L0-SIGFOX Development Board

   Created on: Nov 13, 2019
   Author: Muchiri John
   (c) wwww.warefab.com

   Based on: https://os.mbed.com/users/jurica238814/code/Lis2dh12/

   This software component is licensed by warefab under BSD 3-Clause license,
   the "License"; You may not use this file except in compliance with the
   License. You may obtain a copy of the License at:

   opensource.org/licenses/BSD-3-Clause
*/

#include <stdio.h>

#include "lis2dh12.h"

void Lis2dh12::init(uint8_t addr) {
  address = addr;
  x = 0;
  y = 0;
  z = 0;

  Wire.begin();
  //check if device ready and setup
  uint8_t state = whoIAm();
  if (state == 0x33) {
    int1Setup(INT1_CFG);
    setMode(HIGH_RES);
    setScale(_16g);
    setODR(ODR_10Hz);
    enableAxes(X_axis);
    enableAxes(Y_axis);
    enableAxes(Z_axis);
  }
}

uint8_t Lis2dh12::whoIAm() {

  uint8_t config;
  uint8_t state = 0;

  //char buf[12];

  Wire.beginTransmission(address);
  Wire.write(WHO_AM_I);
  Wire.endTransmission();
  Wire.requestFrom(address, 1);
  if (Wire.available()) {
    config = (uint8_t)Wire.read();
  }
  Wire.endTransmission();
  
  if (config != 0x33) {
    Serial.println("Acc not found");
    return 0;
  }
  return config;
}

uint8_t Lis2dh12::readXYZ() {
  x = readXAxis();
  delay(1);
  y = readYAxis();
  delay(1);
  z = readZAxis();
  delay(1);

  return 1;
}

uint8_t Lis2dh12::setMode(enum Mode_ mode) {
  char ctrl1Copy;
  char ctrl4Copy;
  uint8_t success;

  readFromReg((char) CTRL_REG1, (uint8_t*) &ctrl1Copy, 1);
  readFromReg((char) CTRL_REG4, (uint8_t*) &ctrl4Copy, 1);

  switch (mode) {
    case HIGH_RES:
      ctrl1Copy &= 0xF7;
      ctrl4Copy |= 0x08;
      break;
    case NORMAL:
      ctrl1Copy &= 0xF7;
      ctrl4Copy &= 0xF7;
      break;
    case LOW_POWER:
      ctrl1Copy |= 0x08;
      ctrl4Copy &= 0xF7;
      break;
    default:
      return 0;
  }
  writeToReg((char) CTRL_REG1, (uint8_t*) &ctrl1Copy, 1);
  success = writeToReg((char) CTRL_REG4, (uint8_t*) &ctrl4Copy, 1);
  return success;
}

uint8_t Lis2dh12::enableAxes(enum Axis_ axis) {
  char ctrl1Copy;
  readFromReg((char) CTRL_REG1, (uint8_t*) &ctrl1Copy, 1);
  ctrl1Copy |= axis;
  writeToReg((char) CTRL_REG1, (uint8_t*) &ctrl1Copy, 1);
  return 0;
}

uint8_t Lis2dh12::disableAxes(enum Axis_ axis) {
  char ctrl1Copy;
  readFromReg((char) CTRL_REG1, (uint8_t*) &ctrl1Copy, 1);
  ctrl1Copy &= ~(1 << axis);
  writeToReg((char) CTRL_REG1, (uint8_t*) &ctrl1Copy, 1);
  return 0;
}

int16_t Lis2dh12::readXAxis() {
  int16_t rawData;
  uint8_t tempData;
  // Make sure new data is ready
  do {
    readFromReg((uint8_t) STATUS, (uint8_t*) &tempData, 1);
  } while (!(tempData & 0x08));
  do {
    readFromReg((uint8_t) STATUS, (uint8_t*) &tempData, 1);
  } while (!(tempData & 0x80));
  // Same data have been overwritten

  //readFromReg((char)OUT_X_H, (uint8_t*)&tempData, 1);
  //rawData = (int8_t)tempData << 8;
  readFromReg((uint8_t) OUT_X_L, (uint8_t*) &rawData, 1);
  readFromReg((uint8_t) OUT_X_H, ((uint8_t*) &rawData) + 1, 1);

  if (rawData >= 0)
    rawData = (rawData >> 4);
  else
    rawData = (rawData >> 4) | 0xF000;

  return rawData;
}

int16_t Lis2dh12::readYAxis() {
  int16_t rawData;
  //char tempData;
  //readFromReg((char)OUT_Y_H, (uint8_t*)&tempData, 1);
  //rawData = (int8_t)tempData << 8;
  readFromReg((uint8_t) OUT_Y_L, (uint8_t*) &rawData, 1);
  readFromReg((uint8_t) OUT_Y_H, ((uint8_t*) &rawData) + 1, 1);

  if (rawData >= 0)
    rawData = (rawData >> 4);
  else
    rawData = (rawData >> 4) | 0xF000;

  return rawData;
}

int16_t Lis2dh12::readZAxis() {
  int16_t rawData = 0;
  //char tempData;
  //readFromReg((char)OUT_Z_H, (uint8_t*)&tempData, 1);
  //rawData = (int8_t)tempData << 8;
  readFromReg((uint8_t) OUT_Z_L, (uint8_t*) &rawData, 1);
  readFromReg((uint8_t) OUT_Z_H, ((uint8_t*) &rawData) + 1, 1);

  if (rawData >= 0)
    rawData = (rawData >> 4);
  else
    rawData = (rawData >> 4) | 0xF000;

  return rawData;
}

uint8_t Lis2dh12::setODR(enum Odr_ odr) {
  char ctrl1Copy;
  readFromReg((char) CTRL_REG1, (uint8_t*) &ctrl1Copy, 1);
  ctrl1Copy |= (odr << 4);
  writeToReg((char) CTRL_REG1, (uint8_t*) &ctrl1Copy, 1);
  return 0;
}

uint8_t Lis2dh12::setScale(enum Scale_ scale) {
  char ctrl4Copy;
  readFromReg((char) CTRL_REG4, (uint8_t*) &ctrl4Copy, 1);
  ctrl4Copy |= (scale << 4);
  writeToReg((char) CTRL_REG4, (uint8_t*) &ctrl4Copy, 1);
  return 0;
}

/* Interrupt activity 1 driven to INT1 pad */
uint8_t Lis2dh12::int1Setup(uint8_t setup) {
  char data = setup;
  writeToReg((char) CTRL_REG3, (uint8_t*) &data, 1);
  return 0;
}

uint8_t Lis2dh12::int1Latch(uint8_t enable) {
  char ctrl5Copy;
  readFromReg((char) CTRL_REG5, (uint8_t*) &ctrl5Copy, 1);
  ctrl5Copy |= enable;
  writeToReg((char) CTRL_REG5, (uint8_t*) &ctrl5Copy, 1);
  return 0;
}

uint8_t Lis2dh12::int1Duration(uint8_t duration) {
  char copy = duration;
  writeToReg((char) INT1_DURATION, (uint8_t*) &copy, 1);
  return 0;
}

uint8_t Lis2dh12::int1Threshold(uint8_t threshold) {
  char copy = threshold;
  writeToReg((char) INT1_THS, (uint8_t*) &copy, 1);
  return 0;
}

uint8_t Lis2dh12::int1Config(uint8_t config) {
  char copy = config;
  writeToReg((char) INT1_CFG, (uint8_t*) &copy, 1);
  return 0;
}

void Lis2dh12::clearIntFlag() {
  char data;
  readFromReg((char) INT1_SRC, (uint8_t*) &data, 1);
}

uint8_t Lis2dh12::readFromReg(uint8_t regAddr, uint8_t *buff, uint8_t buffSize) {
  /*HAL_I2C_Mem_Read(&hi2c1, address, WHO_AM_I, 1, &config,
    1, 1500);*/
  //HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout)
  uint8_t retVal = 0;

  Wire.beginTransmission(address);
  Wire.write(regAddr);
  Wire.endTransmission();
  Wire.requestFrom(address, 1);
  if (Wire.available()) {
    retVal = (uint8_t)Wire.read();
  }
  Wire.endTransmission();
  
  *buff = retVal;

  return retVal;
}

uint8_t Lis2dh12::writeToReg(uint8_t regAddr, uint8_t *buff, uint8_t buffSize) {
  uint8_t retVal = 0;

  Wire.beginTransmission(address);
  Wire.write(regAddr);
  retVal = Wire.write(*buff);
  Wire.endTransmission();

  return retVal;
}
