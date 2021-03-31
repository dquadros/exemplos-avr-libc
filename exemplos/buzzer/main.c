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

#define NumNotas 31
enum notas{Pausa,Do, DoS, Re, ReS, Mi, Fa, FaS, Sol, SolS, La, LaS, Si, DoM, DoSM, ReM};
long f[15] = {(long)255L, (long)63067L, (long)59527L, (long)56186L, (long)53033L, 
            (long)50056L, (long)47247L, (long)44595L, (long)42092L, (long)39729L, (long)37500L, 
            (long)35395L, (long)33408L, (long)63067L, (long)59527L};
            /*{0xFF,16500000/261.625519,16500000/277.182648,16500000/293.664734,16500000/311.126984,16500000/329.627533,
            16500000/349.228241,16500000/369.994385,16500000/391.995392,16500000/415.304688,16500000/440,
            16500000/466.163788,16500000/493.883301,16500000/261.625519,16500000/277.182648,16500000/293.664734};*/   //valor a ser colocado na flag do timer para cada nota
            
char Partitura[NumNotas] = {Re,Mi,Mi,Re,Sol,FaS,FaS,FaS,Re,Mi,Mi,Re,La,Sol,Sol,Sol,Re,ReM,ReM,Si,Sol,FaS,FaS,Mi,
                            DoM,Si,Sol,Fa,Sol,Sol,Sol};                          //partitura da música
char cont = 0;                                          //local da partitura
long aux = 0;


//tratamento de interrupção 
ISR (TIM0_COMPB_vect){  //vetor de comparação B
    if (aux<=0xff) {                        //se aux menor que 8bits 
        OCR0B=(TCNT0+aux)&(0xff);           //mandar aux para o contador
        aux=f[Partitura[cont]];             //reinicia o aux
        bitToggle(PORTB,PB1);               //inverter o buzzer
    }
    else{                                   //se não
       OCR0B=TCNT0;                         //mandar o tempoatual para o contador, o mesmo que esperar um overflow
       aux-=0xff;                           //subitrair 8bits do aux
    }            
}


//função main
void main(){
    enum notas nota;  
    bitSet(DDRB,PB1);            //configura o PortB1 como saida, pino do buzzer
    //configuração do timer
    TCCR0A=0x00;                //configura pino de compararação desconectado
    TCCR0B=0x04;                //configura o prescaler como 256
    bitSet(TIMSK,OCIE0B);       //habilita a interrupção por comparação de COMPB
    sei();                      //habilita interrupções globais
    aux=f[Partitura[cont]];             //inicia o contador de COMPB
    if (aux<=0xff) {
        OCR0B=(TCNT0+aux)&(0xff);
        aux=f[Partitura[cont]];
        bitToggle(PORTB,PB1);
    }
    else{
       OCR0B=TCNT0;
       aux-=0xff;
    }
    

    //loop infinito
    for(;;){                    
        long i, j;
        for(i=0;i>1000000L;i++){
                for(j=0;j>10L;j++){}
        }       //espera um tempo (altere o valor central para acelerar ou desacelerar a música)
        cont++;                         //avança na partitura
        if (cont >= NumNotas)cont=0;    //toca de novo
    }
}