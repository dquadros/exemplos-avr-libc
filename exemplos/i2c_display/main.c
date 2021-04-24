/**
 * 
 * @file main.c
 * @author Daniel Quadros
 * @brief Exemplo de uso de I2C para conectar display alfanumérico
 * @version 0.1
 * @date 2021-02-03
 * 
 * Baseado em http://dqsoft.blogspot.com/2015/06/i2c-master-no-attiny25-usi.html
 * porém o display é o descrito em https://dqsoft.blogspot.com/2015/09/outra-opcao-de-display-alfanumerico-com.html
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

// Constantes para comando do display
#define ADDR_DISP   (0x27 << 1)
#define SEL_REG     0x08    // RS=0, RW=0, E=0, BL=1
#define SEL_DADO    0x09    // RS=1, RW=0, E=0, BL=1
#define MSK_E       0x04

// Rotinas
void    DisplayInit     (void);
void    DisplayWriteDec (uint8_t l, uint8_t c, uint8_t num);
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
    uint8_t seg, minuto, hora;
    
    // Inicia I2C
    I2C_Init();
    
    // Configura o display  
    DisplayInit();  

    // Mostra mensagens  
    DisplayWrite (0, 0, "Franzininho");   
    DisplayWrite (1, 0, "00:00:00");   
    
    // Relogio
    seg = minuto = hora = 0;
    for (;;) {
        // Aguarda um segundo
        _delay_ms (1000);
        // Avanca o relogio
        if (++seg == 60)
        {
            seg = 0;
            if (++minuto == 60)
            {
                minuto = 0;
                if (++hora == 100)
                    hora = 0;
            }
        }
        
        // Atualiza o display
        DisplayWriteDec (1, 0, hora);  
        DisplayWriteDec (1, 3, minuto);  
        DisplayWriteDec (1, 6, seg);  
    }        
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
  
// Mostra número decimal de 00 a 99  
void DisplayWriteDec (uint8_t l, uint8_t c, uint8_t num)  
{  
  char aux[3];  
    
  aux[0] = 0x30 + num / 10;  
  aux[1] = 0x30 + num % 10;  
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

