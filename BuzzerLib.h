#ifndef BUZZERLIB_H
#define BUZZERLIB_H

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "NOTES.h"

#define f_clock 16000000 // frecuencia reloj en Hz
#define DC_inicial 50    // duty cycle inicial para el PWM (50%)

// Declaración de funciones de la biblioteca Parlante
void InicializaBuzzer(void); //Inicializa el buzzer configurando el puerto PWM
void musicaBuzzer(int* melodia, int* tempo, int size); // Reproduce melodía a velocidad normal
void musicaBuzzerVelocidadX2(int* melodia, int* tempo, int size); // Nueva función para velocidad x2
void Apaga_PWM(void); //reproduce la música a doble velocidad
void Enciende_PWM(void); //Apaga el PWM y el buzzer
void Actualiza_PWM(uint32_t F_PWM, uint32_t DC); // configura la frecuencia y el duty cycle del PWM
void buzz(long frequency, uint32_t DC); //emite un sonido específico en el buzzer
void Delay_ms(int time_ms); // genera un retardo en milisegundos

#endif // BUZZERLIB_H
