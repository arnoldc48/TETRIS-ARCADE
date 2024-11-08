#ifndef _BUZZER_H_
#define _BUZZER_H_

#include <stdlib.h>
#include <stdint.h>

void Config_Puerto_B(void);
void Config_Puerto_C(void);
void Config_PWM(void);
void Apaga_PWM(void);
void Enciende_PWM(void);
void Actualiza_PWM(uint32_t F_PWM, uint32_t DC);
void Delay_ms(int time_ms);
void buzz(long frequency, uint32_t DC);
void playMelody(int *pMelody, int *pTempo, int size, uint32_t ValDC);

#endif
