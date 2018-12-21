/** \file
  *
  *  \author Atmel Crypto Products
  *
  *  \copyright Copyright (c) 2014 Atmel Corporation. All rights reserved.
  *
  * \atmel_crypto_device_library_license_start
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *
  * 1. Redistributions of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  *
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  *
  * 3. The name of Atmel may not be used to endorse or promote products derived
  *    from this software without specific prior written permission.
  *
  * 4. This software may only be redistributed and used in connection with an
  *    Atmel integrated circuit.
  *
  * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
  * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
  * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
  * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  * POSSIBILITY OF SUCH DAMAGE.
  *
  * \atmel_crypto_device_library_license_stop
  */

#ifndef ATECC_i2c_H_
#define ATECC_i2c_H_

typedef enum {
	I2C_SPEED_100KHZ = 100000,
	I2C_SPEED_400KHZ = 400000,
	I2C_SPEED_1MHZ =  1000000
} i2c_speed_t;

typedef enum {
	I2CBUS_0,
	I2CBUS_1
} i2c_bus_t;


#define CRYPTOAUTH_SLAVE_ADDRESS 0xc0
#define CRYPTOAUTH_BUS I2CBUS_0

// Error codes for physical hardware dependent module
#define I2C_SUCCESS     ((uint8_t) 0x00) //!< Communication with device succeeded.
#define I2C_COMM_FAIL   ((uint8_t) 0xF0) //!< Communication with device failed.
#define I2C_TIMEOUT     ((uint8_t) 0xF1) //!< Communication timed out.
#define I2C_NACK        ((uint8_t) 0xF8) //!< I2C nack

// Function prototypes to be implemented in the target i2c_phys.c
void    i2c_enable(uint32_t speed);
void    i2c_disable(void);
uint8_t i2c_send_start(void);
uint8_t i2c_send_stop(void);
uint8_t i2c_send_bytes(uint8_t count, uint8_t *data);
uint8_t i2c_receive_byte(uint8_t *data);
uint8_t i2c_receive_bytes(uint8_t count, uint8_t *data);
void    i2c_set_address(uint8_t addr);
uint8_t i2c_send_wake(void);

void    phy_i2c_master_enable(i2c_bus_t bus, uint32_t speed);
void    phy_i2c_master_disable(i2c_bus_t bus);
uint8_t phy_i2c_master_send_start(i2c_bus_t bus);
uint8_t phy_i2c_master_send_stop(i2c_bus_t bus);
uint8_t i2c_master_send_bytes(i2c_bus_t bus, uint8_t count, uint8_t *data);
uint8_t i2c_master_receive_byte(i2c_bus_t bus, uint8_t *data);
uint8_t i2c_master_receive_bytes(i2c_bus_t bus, uint8_t count, uint8_t *data);
void    i2c_master_set_slave_address(i2c_bus_t bus, uint8_t addr);
uint8_t i2c_master_send_wake(i2c_bus_t bus);

void atca_set_device_address(i2c_bus_t bus, uint8_t i2caddr);

#endif /* ATECC_i2c_H_ */