/**
 * 
 * @file main.c
 * @author Eduardo Dueñas / Daniel Quadros
 * @brief Exemplo tocar musicas usando buzzer
 * @version 0.2
 * @date 01-04-2021
 * 
 * 
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define bitToggle(endereco,bit) (endereco ^= (1<<bit))
#define bitSet(endereco,bit) (endereco |= (1<<bit))
#define bitClear(endereco,bit) (endereco &= ~(1<<bit))
#define bitTest(endereco,bit) (endereco & (1<<bit))

enum notas {Pausa, Do, DoS, Re, ReS, Mi, Fa, FaS, Sol, SolS, La, LaS, Si, DoM, DoSM, ReM};

// Calcula o valor a programar para a frequencia 'freq' (em decimais de Hz)
// O Timer0 será programado para a frequência F_CPU/256
// A cada contagem a saída muda, portando um ciclo dura 2 interrupções
#define CONT(freq) ((F_CPU*10L)/(2L*256L*freq))

// Frequências conferidas em https://pages.mtu.edu/~suits/notefreqs.html
long f[] = { 255L, CONT(2616L), CONT(2772L), CONT(2937L), CONT(3111L), CONT(3296L), CONT(3492L),
             CONT(3700L), CONT(3920L), CONT(4153L), CONT(440L), CONT(4662L), CONT(4939L),
             CONT(2616L), CONT(2772L), CONT(2937L) };


// Sequencia de notas a tocar
int Partitura[] = {Re,Mi,Mi,Re,Sol,FaS,FaS,FaS,Re,Mi,Mi,Re,La,Sol,Sol,Sol,Re,ReM,ReM,Si,Sol,FaS,FaS,Mi,
                   DoM,Si,Sol,Fa,Sol,Sol,Sol};                          //partitura da música
#define NumNotas    (sizeof(Partitura)/sizeof(Partitura[0]))

volatile uint8_t notaAtual = 0;         //local da partitura

volatile long meio_periodo = 0;         // contador para gerar o período da nota

// Variaveis para controle de delay
static volatile uint16_t cnt_delay;

// Rotinas
static void delayMiliseconds (uint16_t mseg);    
static void delay_init (void);

//tratamento de interrupção do Timer0
ISR (TIM0_COMPB_vect){  //vetor de comparação B
    if (meio_periodo == 0L) {
        // acabou um meio período
        meio_periodo = f[Partitura[notaAtual]];  // recarrega a contagem
        bitToggle(PORTB,PB1);                    // inverter o buzzer
    }
    if (meio_periodo <= 255L) {
        // última parte da contagem
        OCR0B=(TCNT0+meio_periodo)&(0xff);       // aguardar o que resta do meio periodo
        meio_periodo = 0;
    }
    else{                                   //se não
       OCR0B=TCNT0;                         // mandar o tempoatual para o contador, o mesmo que esperar um overflow
       meio_periodo -= 256L;                // subtrair 256 contagens do meio_periodo
    }            
}


//função main
int main(){
    bitSet(DDRB,PB1);            //configura o PortB1 como saida, pino do buzzer
    //configuração do timer
    TCCR0A=0x00;                //configura pino de compararação desconectado
    TCCR0B=0x04;                //configura o prescaler como 256
    bitSet(TIMSK,OCIE0B);       //habilita a interrupção por comparação de COMPB
    meio_periodo = 0;           //força o início de um ciclo
    delay_init();               //inicia contagem de tempo
    sei();                      //habilita interrupções globais

    //loop infinito
    for(;;){
        delayMiliseconds(300);
        cli();
        notaAtual++;                         //avança na partitura
        if (notaAtual >= NumNotas) {
            notaAtual=0;    //toca de novo
        }
        sei();
    }
}

// ----------------------------------------------------------------------------
// Rotinas de Temporização
// ----------------------------------------------------------------------------

// Aguarda (grosseiramente) um certo número de milisegundos (1 a 65534)
static void delayMiliseconds (uint16_t mseg)
{
    cli();
    cnt_delay = mseg + 1;
    sei();
    for (;;)
    {
        cli();
        if (cnt_delay == 0)
            break;
        sei();
    }
    sei();
}

// Inicia a contagem de tempo
static void delay_init (void)
{
    TCCR1  = _BV(CS12)|_BV(CS11)|_BV(CS10);     // Usar CK/64: int a cada 64*256/16500 ms
                                                //   = 0,993 ms
    bitSet(TIMSK,TOIE1);                        // Interromper no overflow
}

// Interrupção do Timer1
ISR(TIM1_OVF_vect) 
{
    if (cnt_delay)
        cnt_delay--;
}
