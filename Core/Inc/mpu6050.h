/*
 * mpu6050.h
 *
 *  Created on: Jun 28, 2026
 *      Author: caden
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include <stdint.h>
#include "main.h"

typedef struct {
	int16_t accel_x, accel_y, accel_z;
	int16_t gyro_x, gyro_y, gyro_z;
} MPU6050_Data_t;

// Initialization
#define MPU6050_ADDR 0x68 << 1
#define MPU6050_PWR_MGMT 0x6B
#define MPU6050_CFG 0x1A
#define MPU6050_SMPLRT_DIV 0x19
#define MPU6050_GYRO_CFG 0x1B
#define MPU6050_ACCEL_CFG 0x1C
#define MPU6050_INT_ENABLE 0x38
#define MPU6050_INT_CFG 0x37

/**
 * Data
 * Note: Read is continuous starting from 0x3B, Notice gap between ACCEL and GYRO
 * Temp sensings occupy these addresses
 */
#define MPU6050_ACCEL_X_H 0x3B
#define MPU6050_ACCEL_X_L 0x3C
#define MPU6050_ACCEL_Y_H 0x3D
#define MPU6050_ACCEL_Y_L 0x3E
#define MPU6050_ACCEL_Z_H 0x3F
#define MPU6050_ACCEL_Z_L 0x40

#define MPU6050_GYRO_X_H 0x43
#define MPU6050_GYRO_X_L 0x44
#define MPU6050_GYRO_Y_H 0x45
#define MPU6050_GYRO_Y_L 0x46
#define MPU6050_GYRO_Z_H 0x47
#define MPU6050_GYRO_Z_L 0x48

// Function Declarations
HAL_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c);
void MPU6050_Parse(volatile uint8_t* raw_buf, volatile MPU6050_Data_t *latest);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void MPU6050_ReadAll_IT(I2C_HandleTypeDef *hi2c);
uint8_t get_i2c_ready();
void reset_i2c_ready();



#endif /* INC_MPU6050_H_ */
