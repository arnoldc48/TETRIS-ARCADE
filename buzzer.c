#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "buzzer.h"

#define f_clock 16000000 // 16MHz
#define DC_inicial 50    // 50% Duty Cycle inicial

//**************************************************************************************************************
//******************* Configurando el Puerto B pin 4 para funciones especiales *********************************
//**************************************************************************************************************
void Config_Puerto_B(void) {
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // Habilitamos Puerto B (PB)
    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1)); // Esperamos a que se active
    GPIO_PORTB_AMSEL_R &= ~(0X10); // Desactivando funciones analÃƒÂ³gicas para PB4
    GPIO_PORTB_AFSEL_R |= 0X10;    // Seleccionando PB4 para funciones especiales
    GPIO_PORTB_DIR_R |= 0X10;      // PB4 como salida
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0XFFF0FFFF) | 0X70000; // PB4 como T1CCP0
    GPIO_PORTB_DEN_R |= 0X10;      // Habilitando PB4
}

void Config_Puerto_C(void) {
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2; // Activa el puerto C
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R2) == 0); // Espera a que se active
    GPIO_PORTC_DIR_R &= ~0xF0; // Configura PC7, PC6, PC5, PC4 como entrada
    GPIO_PORTC_DEN_R |= 0xF0; // Habilita PC7, PC6, PC5, PC4 como digital
    GPIO_PORTC_PUR_R |= 0xF0; // Activa el pull-up en PC7, PC6, PC5, PC4
}

//**************************************************************************************************************
//********************* Configurando el TIMER1 para ser utilizado como PWM *************************************
//**************************************************************************************************************
void Config_PWM(void) {
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1; // Activa el reloj para TIMER1
    while (!(SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R1)); // Espera a que se active
    TIMER1_CTL_R &= ~(0x0101); // Desactiva el Timer, señal no invertida, modo descendente
    TIMER1_CFG_R = 0x04; // Selecciona modo de 16 bits
    TIMER1_TAMR_R = 0x0A; // Configura en modo PWM, cuenta por flancos y periódico
}

void Apaga_PWM(void) {
    TIMER1_CTL_R &= ~(0x0101); // Apaga el PWM completamente
    GPIO_PORTB_DATA_R &= ~0x10; // Asegura que PB4 esté en baja
}

void Enciende_PWM(void) {
    TIMER1_CTL_R |= 0x0101; // Enciende el PWM
}

//***********************************************************************************************************
//************** Funciónn que actualizara la frecuencia y el duty cycle del PWM *****************************
//***********************************************************************************************************
void Actualiza_PWM(uint32_t F_PWM, uint32_t DC) {
    uint32_t ValorTope = (f_clock / F_PWM) - 1; // Calcula el valor tope
    uint32_t ValorDC = (((100 - DC) * ValorTope) / 100); // Calcula el valor comparador para el DC
    TIMER1_TAPR_R = (ValorTope & 0xFF0000) >> 16; // Ajusta los 8 bits más significativos
    TIMER1_TAILR_R = (ValorTope & 0x00FFFF); // Ajusta los 16 bits menos significativos
    TIMER1_TAPMR_R = (ValorDC & 0xFF0000) >> 16; // Ajusta los 8 bits más significativos para el DC
    TIMER1_TAMATCHR_R = (ValorDC & 0x00FFFF); // Ajusta los 16 bits menos significativos para el DC
}

//***********************************************************************************************************
//************** This function generates delay in ms *******************************************************/
//************** calculations are based on 16MHz system clock frequency ************************************/
//***********************************************************************************************************
void Delay_ms(int time_ms) {
    int i, j;
    for (i = 0; i < time_ms; i++)
        for (j = 0; j < 3180; j++);
}

//***********************************************************************************************************
//************** Función que hace sonar en el buzzer una nota determinada **********************************
//***********************************************************************************************************
void buzz(long frequency, uint32_t DC) {
    if (frequency == 0) {
        Apaga_PWM(); // Apaga completamente el PWM si frecuencia=0
    } else {
        Actualiza_PWM(frequency, DC); // Actualiza frecuencia y ciclo de trabajo
        Enciende_PWM(); // Enciende el PWM con la frecuencia establecida
    }
}

//***********************************************************************************************************
//************** Función que hace sonar una melodia determinada ********************************************
//***********************************************************************************************************
void playMelody(int *pMelody, int *pTempo, int size, uint32_t ValDC) {
    int thisNote = 0;
    while (1) {
        if ((GPIO_PORTC_DATA_R & 0x40) == 0) { // Detiene la música si SW2 (PC6) está presionado
            buzz(0, ValDC); // Apaga el buzzer completamente
            return;
        }

        int noteDuration = 1000 / pTempo[thisNote]; // Calcula duración de la nota
        buzz(pMelody[thisNote], ValDC); // Reproduce la nota actual
        Delay_ms(noteDuration); // Espera la duración de la nota
        thisNote = (thisNote + 1) % size; // Pasa a la siguiente nota y reinicia si llega al final
    }
}
