/*
 * I2c.c - Rotinas para uso do I2C Master
 *
 * Created: 22/04/2021
 * Author:  daniel quadros
 *
 * Estas rotinas são de "baixo nível", tratando do envio e recepção byte a byte
 * As rotinas usam polling e bloqueiam durante a transmissão e recepção
 * Baseado em http://dqsoft.blogspot.com/2015/06/i2c-master-no-attiny25-usi.html
 *
 * Por enquanto apenas o ATtinyx5 é suportado
 * Velocidade fixa 1/8uS = 125KHz
 *
 * @copyright Franzininho 
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 * 
*/ 

#include "I2c.h"

#include <avr/io.h>
#include <util/delay.h>

// Pinos para comunicação I2C
// A USI do ATtinyx5 requer o uso de E/S digital
#define DDR_USI             DDRB
#define PORT_USI            PORTB
#define PIN_USI             PINB
#define PORT_USI_SDA        PORTB0
#define PORT_USI_SCL        PORTB2
#define PIN_USI_SDA         PINB0
#define PIN_USI_SCL         PINB2

// Delay em cada pulso do clock
#define DLY_SCL 5

// Inicia a USI no modo I2C
void I2C_Init ()
{
    USIDR =  0xFF;
    USICR =  _BV(USIWM1) | _BV(USICS1) | _BV(USICLK);   // Sem interrupções
                                                        // Modo TWI
    USISR = _BV(USISIF) | _BV(USIOIF) |                 // Limpa flags e zera contador
            _BV(USIPF) | _BV(USIDC);
    DDR_USI  |= _BV(PIN_USI_SDA) | _BV(PIN_USI_SCL);    // colocar como saída
    PORT_USI |= _BV(PIN_USI_SDA) | _BV(PIN_USI_SCL);    // repouso = high (solto)
}

// Sinaliza Start
void I2C_Start ()
{
    PORT_USI |= _BV(PIN_USI_SCL);               // Solta SCL
    while ((PIN_USI & _BV(PIN_USI_SCL)) == 0)   // Espera SCL alto
        ;
    _delay_us (DLY_SCL);
    PORT_USI &= ~_BV(PIN_USI_SDA);          // Baixa SDA
    _delay_us (DLY_SCL);
    PORT_USI &= ~_BV(PIN_USI_SCL);          // Baixa SCL
    PORT_USI |= _BV(PIN_USI_SDA);           // Solta SDA
}

// Envia um uint8_t e lê o ACK ou NAK
uint8_t I2C_Write (uint8_t b)
{
    uint8_t temp;
    
    PORT_USI &= ~_BV(PIN_USI_SCL);          // Baixa SCL
    USIDR = b;                              // Coloca dado no shift register
    USISR = _BV(USISIF) | _BV(USIOIF) |     // Limpa flags e zera contador
            _BV(USIPF) | _BV(USIDC);
    do
    {
        _delay_us (DLY_SCL);
        USICR |= _BV(USICLK) | _BV(USITC);          // Pulsa clock p/ cima
        while ((PIN_USI & _BV(PIN_USI_SCL)) == 0)   // Espera SCL subir
            ;
        _delay_us (DLY_SCL);              
        USICR |= _BV(USICLK) | _BV(USITC);          // Pulsa clock p/ baixo
    } while ((USISR & _BV(USIOIF)) == 0);       // Repete para todos os bits

    _delay_us (DLY_SCL);                
    USIDR = 0xFF;                            // Solta SDA
    DDR_USI &= ~_BV(PIN_USI_SDA);            // SDA passa a ser entrada
    USISR = _BV(USISIF) | _BV(USIOIF) |      // Limpa flags e inicia contador
            _BV(USIPF) | _BV(USIDC) |        //   para ler 1 bit
            (0xE<<USICNT0);
    do
    {
        _delay_us (DLY_SCL);
        USICR |= _BV(USICLK) | _BV(USITC);          // Pulsa clock p/ cima
        while ((PIN_USI & _BV(PIN_USI_SCL)) == 0)   // Espera SCL subir
            ;
        _delay_us(DLY_SCL);              
        USICR |= _BV(USICLK) | _BV(USITC);          // Pulsa clock p/ baixo
    } while ((USISR & _BV(USIOIF)) == 0);    // Repete para todos os bits
    temp  = USIDR;                           // Lê a resposta
    USIDR = 0xFF;                            // Solta SDA
    DDR_USI |= _BV(PIN_USI_SDA);             // SDA passa a ser saída
    return temp;
}

// Recebe um uint8_t e envia o "ACK" ou "NOACK"
uint8_t I2C_Read (uint8_t last)
{
    uint8_t lido;
    
    PORT_USI &= ~_BV(PIN_USI_SCL);          // Baixa SCL
    USIDR = 0xFF;                           // Solta SDA
    DDR_USI &= ~_BV(PIN_USI_SDA);           // SDA passa a ser entrada
    USISR = _BV(USISIF) | _BV(USIOIF) |     // Limpa flags e zera contador
            _BV(USIPF) | _BV(USIDC);
    do
    {
        _delay_us (DLY_SCL);
        USICR |= _BV(USICLK) | _BV(USITC);          // Pulsa clock p/ cima
        while ((PIN_USI & _BV(PIN_USI_SCL)) == 0)   // Espera SCL subir
            ;
        _delay_us(DLY_SCL);              
        USICR |= _BV(USICLK) | _BV(USITC);  // Pulsa clock p/ baixo
    } while ((USISR & _BV(USIOIF)) == 0);   // Repete para todos os bits
    lido = USIDR;                           // Lê a resposta
    
    // Mestre indica a recepção com um "ACK" ou "No ACK"
    USIDR = 0xFF;                            // Solta SDA
    DDR_USI |= _BV(PIN_USI_SDA);             // SDA passa a ser saída
    if (!last) {
        USIDR = 0x00;                        // "ACK"
    }
    USISR = _BV(USISIF) | _BV(USIOIF) |      // Limpa flags e inicia contador
            _BV(USIPF) | _BV(USIDC) |        //   para enviar 1 bit
            (0xE<<USICNT0);
    do
    {
        _delay_us (DLY_SCL);
        USICR |= _BV(USICLK) | _BV(USITC);          // Pulsa clock p/ cima
        while ((PIN_USI & _BV(PIN_USI_SCL)) == 0)   // Espera SCL subir
            ;
        _delay_us(DLY_SCL);              
        USICR |= _BV(USICLK) | _BV(USITC);          // Pulsa clock p/ baixo
    } while ((USISR & _BV(USIOIF)) == 0);    // Repete para todos os bits
    USIDR = 0xFF;                            // Solta SDA
    
    return lido;
}

// Sinaliza Stop
void I2C_Stop ()
{
    PORT_USI &= ~_BV(PIN_USI_SDA);          // Baixa SDA
    PORT_USI |= _BV(PIN_USI_SCL);           // Solta SCL
    while ((PIN_USI & _BV(PIN_USI_SCL)) == 0)   // Espera SCL alto
        ;
    _delay_us (DLY_SCL);
    PORT_USI |= _BV(PIN_USI_SDA);           // Solta SDA
    _delay_us (DLY_SCL);
}

