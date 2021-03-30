/**
 * 
 * @file main.c
 * @author Eduardo Dueñas
 * @brief Exemplo tocar musicas usando buzzer
 * @version 0.1
 * @date 29-03-2021
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
enum notas{Pausa,Do, DoS, Re, ReS, Mi, Fa, FaS, Sol, SolS, La, LaS, Si, DoM, DoSM, ReM};
long f[15] = {0xFF,16500000/261.625519,16500000/277.182648,16500000/293.664734,16500000/311.126984,16500000/329.627533,
            16500000/349.228241,16500000/369.994385,16500000/391.995392,16500000/415.304688,16500000/440,
            16500000/466.163788,16500000/493.883301,16500000/261.625519,16500000/277.182648,16500000/293.664734};   //valor a ser colocado na flag do timer para cada nota
char Partitura[NumNotas] = {Re,Mi,Mi,Re,Sol,FaS,FaS,FaS,Re,Mi,Mi,Re,La,Sol,Sol,Sol,Re,ReM,ReM,Si,Sol,FaS,FaS,Mi,
                            DoM,Si,Sol,Fa,Sol,Sol,Sol};                          //partitura da música
char cont = 0;                                          //local da partitura


//tratamento de interrupção 
ISR (TIM0_COMPB_vect){  //vetor de comparação B
    OCR0B=(TCNT0+f[Partitura[cont]])&(0xff);            //reinicia o contador de COMPB
}


//função main
void main(){
    enum notas nota;  
    bitSet(DDRB,PB1);            //configura o PortB1 como saida, pino do buzzer

    //configuração do timer
    TCCR0A=0x10;                //configura o resultado da comparação de COMPB como toggle do pino PB1
    TCCR0B=0x04;                //configura o prescaler como 256
    bitSet(TIMSK,OCIE0B);       //habilita a interrupção por comparação de COMPB
    sei();                      //habilita interrupções globais
    OCR0B=(TCNT0+f[Partitura[cont]])&(0xff);             //inicia o contador de COMPB

    //loop infinito
    for(;;){                    
        long i;
        for(i=0;i>1000000;i++){}         //espera um tempo )altere o valor central para acelerar ou desacelerar a música)
        cont++;                         //avança na partitura
        if (cont >= NumNotas)cont=0;    //toca de novo
    }
}