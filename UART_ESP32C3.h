#ifndef UART_ESP32C3_H
#define UART_ESP32C3_H

#include <stdint.h>
#include <stdio.h>    // Asegura que sprintf esté declarado
#include "tm4c123gh6pm.h"

void UART1_Init(void); // Inicializar el UART con el ESP32C3
void UART1_WriteChar(char data);
void UART1_Transmite_Cadena(char Arreglo[]);
void IntToCharArray(int num, char* str); // Pasar valores int a arreglo char para poderlo enviar como cadena

#endif // UART_ESP32C3_H
