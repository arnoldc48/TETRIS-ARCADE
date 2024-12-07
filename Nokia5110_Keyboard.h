#ifndef NOKIA5110_KEYBOARD_H
#define NOKIA5110_KEYBOARD_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "Nokia5110.h"

void actualizarContadorPantalla(void);
void delay(uint32_t ms);
void drawKeyboard(void);
void drawUser(void);
void blinkLetter(void);
void updateCursor(void);
void getUser(char arr[]);

#endif // NOKIA5110_KEYBOARD_H
