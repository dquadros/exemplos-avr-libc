/**
 * 
 * @file main.c
 * @author Eduardo Dueñas
 * @brief Exemplo tocar musicas usando buzzer
 * @version 0.1
 * @date 25-03-2021
 * 
 * 
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16500000L


#define bitToggle(endereco,bit) (endereco ^= (1<<bit))
#define bitSet(endereco,bit) (endereco |= (1<<bit))
#define bitClear(endereco,bit) (endereco &= ~(1<<bit))
#define bitTest(endereco,bit) (endereco & (1<<bit))

#define NumNotas 10
enum notas{Do, DoSustenudo, Re, ReSustenudo, Mi, Fa, FaSustenudo, Sol, SolSustenudo, La, LaSustenudo, Si}



void main(){
    enum notas nota;



}