/*
 * mpu6050.c
 *
 *  Created on: Jun 29, 2026
 *      Author: caden
 */

#include "mpu6050.h"

extern I2C_HandleTypeDef hi2c1;

#define MPU6050_TIMEOUT 10
#define WINDOW_SIZE 32

static volatile uint8_t i2c_ready = 0;
static volatile MPU6050_Data_t history[WINDOW_SIZE];
static uint8_t write_idx = 0;
static volatile uint8_t raw_buf[14] = {0};
static MPU6050_Data_t latest;

/**
 * gets i2c_ready
 */
uint8_t get_i2c_ready() {
	return i2c_ready;
}

void reset_i2c_ready() {
	i2c_ready = 0;
}

/**
 * Parses raw buffer into data struct
 * Recall that idx 6 and 7 store temperature data
 */
void MPU6050_Parse(volatile uint8_t* buf, volatile MPU6050_Data_t *out) {
	out->accel_x = (uint16_t) (buf[0] << 8  | buf[1]);
	out->accel_y = (uint16_t) (buf[2] << 8  | buf[3]);
	out->accel_z = (uint16_t) (buf[4] << 8  | buf[5]); // SKIP TEMP DATA
	out->gyro_x =  (uint16_t) (buf[8] << 8  | buf[9]);
	out->gyro_y =  (uint16_t) (buf[10] << 8 | buf[11]);
	out->gyro_z =  (uint16_t) (buf[12] << 8 | buf[13]);
}


/**
 * Activates upon I2C read transaction completion
 * Triggers parsing of raw data into a data structure
 * Parses to current write index, then moves to nex
 */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
        MPU6050_Parse(raw_buf, &history[write_idx]);
        write_idx = (write_idx + 1) % WINDOW_SIZE;
    }
}


/**
 * Activates upon INT pin
 * Triggers MPU6050 I2C read transaction
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_8) {
		i2c_ready = 1;
	}
}


/**
 * Initializes MPU6050 with preset configuration
 * Returns HAL_OK
 */
HAL_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c) {
	uint8_t pwr = (1 << 0);
	uint8_t cfg = (3 << 0);
	uint8_t smplrt = (9 << 0); // 1000 / (1+9) = 100 Hz SMPRT
	uint8_t gyro = (1 << 3);
	uint8_t accel = (2 << 3);
	uint8_t int_enable = (1 << 0);
	uint8_t int_cfg = (1 << 4);

	HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, MPU6050_PWR_MGMT, I2C_MEMADD_SIZE_8BIT, &pwr, 1, MPU6050_TIMEOUT);
	HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, MPU6050_CFG, I2C_MEMADD_SIZE_8BIT, &cfg, 1, MPU6050_TIMEOUT);
	HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, MPU6050_SMPLRT_DIV, I2C_MEMADD_SIZE_8BIT, &smplrt, 1, MPU6050_TIMEOUT);
	HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, MPU6050_GYRO_CFG, I2C_MEMADD_SIZE_8BIT, &gyro, 1, MPU6050_TIMEOUT);
	HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, MPU6050_ACCEL_CFG, I2C_MEMADD_SIZE_8BIT, &accel, 1, MPU6050_TIMEOUT);
	HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, MPU6050_INT_ENABLE, I2C_MEMADD_SIZE_8BIT, &int_enable, 1, MPU6050_TIMEOUT);
	HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, MPU6050_INT_CFG, I2C_MEMADD_SIZE_8BIT, &int_cfg, 1, MPU6050_TIMEOUT);
	return HAL_OK;
}


/**
 * Takes 14 readings starting from Accel_X_High up to Gyro_Z_Low
 * Temperature occupies idx 6 and 7
 *
 * Parameters:
 * hi2c - pointer to I2C handle
 * buf - pointer to raw readings buffer
 */
void MPU6050_ReadAll_IT(I2C_HandleTypeDef *hi2c) {
	HAL_StatusTypeDef ret = HAL_I2C_Mem_Read_IT(hi2c, MPU6050_ADDR, MPU6050_ACCEL_X_H, I2C_MEMADD_SIZE_8BIT, raw_buf, 14);
}
