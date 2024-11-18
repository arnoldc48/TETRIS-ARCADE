#include "BuzzerLib.h" // Incluye el encabezado de la biblioteca para el buzzer

// Configuración del Duty Cycle inicial para el PWM
static int ValDC = DC_inicial; // Define un Duty Cycle inicial del 50% para el PWM

// Configura el buzzer inicializando el puerto y el PWM
void InicializaBuzzer(void) {
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // Habilita el reloj para el puerto B
    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1)); // Espera hasta que el puerto B esté listo
    GPIO_PORTB_AMSEL_R &= ~0x10; // Desactiva las funciones analógicas en PB4
    GPIO_PORTB_AFSEL_R |= 0x10;  // Habilita funciones alternativas en PB4
    GPIO_PORTB_DIR_R |= 0x10;    // Configura PB4 como salida
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFF0FFFF) | 0x70000; // Configura PB4 para T1CCP0 (PWM)
    GPIO_PORTB_DEN_R |= 0x10;    // Habilita la función digital en PB4

    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1; // Habilita el reloj para TIMER1
    while (!(SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R1)); // Espera hasta que TIMER1 esté listo
    TIMER1_CTL_R &= ~0x0101;     // Desactiva TIMER1 durante la configuración
    TIMER1_CFG_R = 0x04;         // Configura el temporizador en modo de 16 bits
    TIMER1_TAMR_R = 0x0A;        // Configura TIMER1 en modo PWM, cuenta descendente
}

// Apaga el PWM y el buzzer
void Apaga_PWM(void) {
    TIMER1_CTL_R &= ~0x0101;     // Apaga TIMER1 y detiene el PWM
    GPIO_PORTB_DATA_R &= ~0x10;  // Asegura que PB4 esté en bajo para detener el buzzer
}

// Enciende el PWM en el buzzer
void Enciende_PWM(void) {
    TIMER1_CTL_R |= 0x0101;      // Enciende TIMER1 para activar el PWM
}

// Configura la frecuencia y el Duty Cycle del PWM
void Actualiza_PWM(uint32_t F_PWM, uint32_t DC) {
    uint32_t ValorTope = (f_clock / F_PWM) - 1; // Calcula el valor máximo del contador según la frecuencia
    uint32_t ValorDC = (((100 - DC) * ValorTope) / 100); // Calcula el valor para el Duty Cycle

    TIMER1_TAPR_R = (ValorTope & 0xFF0000) >> 16; // Configura los bits más altos del valor tope
    TIMER1_TAILR_R = (ValorTope & 0x00FFFF);      // Configura los bits bajos del valor tope
    TIMER1_TAPMR_R = (ValorDC & 0xFF0000) >> 16;  // Configura los bits altos del Duty Cycle
    TIMER1_TAMATCHR_R = (ValorDC & 0x00FFFF);     // Configura los bits bajos del Duty Cycle
}

// Genera un retardo en milisegundos
void Delay_ms(int time_ms) {
    volatile int i, j; // Volatile asegura que el bucle no sea optimizado
    for (i = 0; i < time_ms; i++) {
        for (j = 0; j < 3180; j++) {} // Bucle vacío para crear el retardo
    }
}

// Emite un tono específico en el buzzer
void buzz(long frequency, uint32_t DC) {
    if (frequency == 0) {
        Apaga_PWM(); // Si la frecuencia es 0, apaga el buzzer
    } else {
        Actualiza_PWM(frequency, DC); // Configura el PWM con la frecuencia y el Duty Cycle
        Enciende_PWM();               // Enciende el PWM para emitir el sonido
    }
}

// Reproduce una melodía a velocidad normal
void musicaBuzzer(int* melodia, int* tempo, int size) {
    int i;
    for (i = 0; i < size; i++) {
        int noteDuration = 1000 / tempo[i]; // Calcula la duración de la nota en ms
        buzz(melodia[i], ValDC);            // Emite la nota actual con el Duty Cycle inicial
        Delay_ms(noteDuration);             // Espera la duración de la nota
    }
    buzz(0, ValDC); // Apaga el buzzer al terminar la melodía
}

// Reproduce una melodía a velocidad x2
void musicaBuzzerVelocidadX2(int* melodia, int* tempo, int size) {
    int i;
    for (i = 0; i < size; i++) {
        int noteDuration = (1000 / tempo[i]) / 4; // Divide la duración para hacerla más rápida
        buzz(melodia[i], ValDC);                  // Emite la nota actual con el Duty Cycle inicial
        Delay_ms(noteDuration);                   // Espera la duración reducida de la nota

        Apaga_PWM();      // Apaga el PWM brevemente entre notas
        Delay_ms(10);     // Pausa breve entre notas para evitar ruidos
    }
    buzz(0, ValDC); // Apaga el buzzer al terminar la musica
}
