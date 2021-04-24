/**
 * 
 * @file main.c
 * @author Daniel Quadros
 * @brief Exemplo de uso de I2C para conectar um RTC e display alfanumérico
 * @version 0.1
 * @date 2021-02-03
 * 
 * Baseado em http://dqsoft.blogspot.com/2020/04/relogio-de-tempo-real-ds1307.html
 * O RTC é um DS1307 num módulo TinyRTC
 * A parte do display é copiada do exemplo i2c_display
 *
 * Conexoes:
 *
 * Franzininho   RTC    Display
 *     GND       GND    GND
 *     Vcc       Vcc    Vcc
 *     P2        SCL    SCL
 *     P0        SDA    SDA
 *
 * @copyright Franzininho 
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 * 
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "../LibFranzininho/Driver/I2c.h"

#define TRUE    1
#define FALSE   0

// Constantes para comando do display
#define ADDR_DISP   (0x27 << 1)
#define SEL_REG     0x08    // RS=0, RW=0, E=0, BL=1
#define SEL_DADO    0x09    // RS=1, RW=0, E=0, BL=1
#define MSK_E       0x04

// Constantes para acesso ao RTC
#define ADDR_RTC   (0x68 << 1)

// Rotinas
void    RtcInit         (void);
void    RtcRead         (uint8_t *buf);
void    RtcWrite        (uint8_t *buf);
void    DisplayInit     (void);
void    DisplayWriteHex (uint8_t l, uint8_t c, uint8_t num);
void    DisplayWrite    (uint8_t l, uint8_t c, char *msg);
void    DisplayOut      (uint8_t c, uint8_t d);
void    DisplaySend     (uint8_t b);

/**
 * @brief Função main
 * 
 * @return int 
 */
int main(void) 
{
    uint8_t bufRTC[7];
    
    // Inicia I2C
    I2C_Init();
    
    // Inicia o RTC
    RtcInit();
    
    // Descomentar as linhas abaixo para gravar a data e hora no RTC
    /*
    bufRTC[0] = 0x80;   // 0x80 | segundos em BCD
    bufRTC[1] = 0x44;   // minutos em BCD
    bufRTC[2] = 0x17;   // hora em BCD
    bufRTC[3] = 0x01;   // dia da semana, 1 a 7
    bufRTC[4] = 0x24;   // dia do mes em BCD
    bufRTC[5] = 0x04;   // mes em BCD
    bufRTC[6] = 0x21;   // ano em BCD (00 a 99)
    RtcWrite (bufRTC);
    */
    
    // Configura o display  
    DisplayInit();  
    DisplayWrite (0, 0, "Franzininho");   
    DisplayWrite (1, 0, "00/00 00:00:00");   

    // Laço Principal
    for (;;) {
        _delay_ms (1000);
        RtcRead (bufRTC);
        DisplayWriteHex (1, 0, bufRTC[4]& 0x3F);
        DisplayWriteHex (1, 3, bufRTC[5]& 0x1F);
        DisplayWriteHex (1, 6, bufRTC[2]& 0x3F);
        DisplayWriteHex (1, 9, bufRTC[1]& 0x7F);
        DisplayWriteHex (1, 12, bufRTC[0]& 0x7F);  
    }
}

// Inicia o RTC
void RtcInit (void) {
    uint8_t reg0;
    
    // Seleciona o primeiro registrador
    I2C_Start ();
    I2C_Write (ADDR_RTC);
    I2C_Write (0);          // endereço do primeiro registrador
    I2C_Stop ();
    
    // Faz a leitura
    I2C_Start ();
    I2C_Write (ADDR_RTC | 1);   // leitura
    reg0 = I2C_Read (TRUE);
    I2C_Stop ();
    
    // Liga o clock se estava parado
    if (reg0 & 0x80) {
        I2C_Start ();
        I2C_Write (ADDR_RTC);
        I2C_Write (0);
        I2C_Write (reg0 & 0x7F);
        I2C_Stop ();
    }
}

// Lê os primeiro sete registradores do RTC
// (segundos, minutos, hora, dia da semana, data, mês e ano)
void RtcRead (uint8_t *buf) {
    uint8_t i;
    
    // Seleciona o primeiro registrador
    I2C_Start ();
    I2C_Write (ADDR_RTC);
    I2C_Write (0);          // endereço do primeiro registrador
    I2C_Stop ();
    // Faz a leitura
    I2C_Start ();
    I2C_Write (ADDR_RTC | 1);   // leitura
    for (i = 0; i < 7; i++) {
        buf[i] = I2C_Read (i == 6);
    }
    I2C_Stop ();
}

// Atualiza os primeiro sete registradores do RTC
// (segundos, minutos, hora, dia da semana, data, mês e ano)
void RtcWrite (uint8_t *buf) {
    uint8_t i;
    
    // Seleciona o primeiro registrador
    I2C_Start ();
    I2C_Write (ADDR_RTC);
    I2C_Write (0);          // endereço do primeiro registrador
    // Faz a escrita
    for (i = 0; i < 7; i++) {
        I2C_Write (buf[i]);
    }
    I2C_Stop ();
}

// Faz a iniciação do display  
void DisplayInit ()  
{
  // Coloca os sinais em situação de repouso
  DisplaySend (0x00);

  // Sequência que coloca em um modo conhecido
  _delay_ms(100);
  DisplayOut (SEL_REG, 0x03);  
  _delay_ms(5);
  DisplayOut (SEL_REG, 0x03);  
  _delay_ms(1);
  DisplayOut (SEL_REG, 0x03);  
  _delay_ms(1);
  DisplayOut (SEL_REG, 0x02);
  _delay_ms(1);
  
  // Programa o moco
  DisplayOut (SEL_REG, 0x28);
  _delay_ms(1);
    
  // Limpa a tela
  DisplayOut (SEL_REG, 0x01);  
  _delay_ms(2);
    
  // Liga o display
  DisplayOut (SEL_REG, 0x0C);  
}  

// Mostra número hexadecimal (ou em BCD)
static const char hexa[] = "0123456789ABCDEF";
void DisplayWriteHex (uint8_t l, uint8_t c, uint8_t num) {  
  char aux[3];  
    
  aux[0] = hexa[num >> 4];
  aux[1] = hexa[num & 0x0F];
  aux[2] = 0;  
  DisplayWrite (l, c, aux);  
}  
  
// Posiciona o cursor e mostra mensagem  
void DisplayWrite (uint8_t l, uint8_t c, char *msg)  
{  
  uint8_t addr = c;  
  if (l == 1)  
    addr += 0x40;  
  DisplayOut (SEL_REG, 0x80 + addr);  
  while (*msg)  
  {  
    DisplayOut (SEL_DADO, *msg++);  
  }  
}  
  
// Envia um uint8_t de controle e um uint8_t de dado  
void DisplayOut (uint8_t c, uint8_t d)  
{
    // Seleciona Reg ou dado
    DisplaySend(c);
    
    // Levanta E, envia nibble superior, abaixa E
    DisplaySend(c | MSK_E);
    DisplaySend(c | MSK_E | (d & 0xF0));
    DisplaySend(c | (d & 0xF0));
    
    // Levanta E, envia nibble inferior, abaixa E
    DisplaySend(c | MSK_E);
    DisplaySend(c | MSK_E | (d << 4));
    DisplaySend(c | (d << 4));
}  

// Envia um byte ao display
void DisplaySend (uint8_t b) {
    I2C_Start ();
    I2C_Write (ADDR_DISP);
    I2C_Write (b);
    I2C_Stop ();
}

