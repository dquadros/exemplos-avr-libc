/*
 * I2c.h - Rotinas para uso do I2C Master
 *
 * Created: 22/04/2021
 * Author:  daniel quadros
 *
 * Estas rotinas são de "baixo nível", tratando do envio e recepção byte a byte
 * Baseado em http://dqsoft.blogspot.com/2015/06/i2c-master-no-attiny25-usi.html
 *
*/ 

#include <stdint.h>

void    I2C_Init        (void);
void    I2C_Start       (void);
uint8_t I2C_Write       (uint8_t b);
uint8_t I2C_Read        (uint8_t last);
void    I2C_Stop        (void);

