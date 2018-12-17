/*
 * conf_sensor.h
 *
 * Created: 11/3/2015 4:19:49 PM
 *  Author: gracy.angela
 */ 


#ifndef CONF_SENSOR_H_
#define CONF_SENSOR_H_

/* SERCOM - I2C Instance */
extern struct i2c_master_module i2c_master_instance;

// SENSOR I2C defines
#define SENSOR_I2C               SERCOM4
#define SENSOR_I2C_PINMUX_PAD0   PINMUX_PA12D_SERCOM4_PAD0
#define SENSOR_I2C_PINMUX_PAD1   PINMUX_PA13D_SERCOM4_PAD1

// #define SENSOR_I2C               SERCOM2
// #define SENSOR_I2C_PINMUX_PAD0   PINMUX_PA12C_SERCOM2_PAD0
// #define SENSOR_I2C_PINMUX_PAD1   PINMUX_PA13C_SERCOM2_PAD1



#endif /* CONF_SENSOR_H_ */