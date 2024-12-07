#include "Nokia5110_Keyboard.h"

// Definiciones de constantes
#define SCREEN_WIDTH 48
#define SCREEN_HEIGHT 84
#define BOARD_WIDTH 21
#define BOARD_HEIGHT 10

// Definiciones de los switches

#define SW1 0x80
#define SW2 0x40
#define SW3 0x20
#define SW4 0x10

uint16_t contadorPantalla = 0;

// Función para actualizar el contador de tiempo
void actualizarContadorPantalla(void) {
//preguntamos si paso 1ms
    if((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT)) {
        //cada vez que pase 1ms, el contador de tiempo se incrementa
        contadorPantalla++;
        if(contadorPantalla == 160){
            contadorPantalla = 0;
        }
    }
}

char user[11] = {' '};
uint8_t cursor_row = 0;
uint8_t cursor_col = 0;

int user_count = 0;

char keyboard[3][10] = {
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'},
    {'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'},
    {'U', 'V', 'W', 'X', 'Y', 'Z', '-', '+'},
};

void delay(uint32_t ms) {
    volatile uint32_t i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 3180; j++) {
            // No hacer nada, simplemente retraso aproximado
        }
    }
}

void drawKeyboard(void) {
    clearBuffer(); //limpiamos el buffer
    //todo el teclado encendido
    text(0, 4,  (unsigned char *)"ENTER YOUR NAME:", 0);
    text(5, 17,  (unsigned char *)"_ _ _ _ _ _ _ _ _ _", 0);
    text(0, 27,  (unsigned char *)"A B C D E F G H I J", 0);
    text(0, 35,  (unsigned char *)"K L M N O P Q R S T", 0);
    text(0, 43,  (unsigned char *)"U V W X Y Z     - +", 0);
}

uint8_t user_col = 5;
uint8_t user_row = 13;

void drawUser(void) {
    char temp[2];
    user_col = 5;  // Columna inicial
    user_row = 13; // Fila fija para el texto

    for (int i = 0; i < 10; i++) {
        if (user[i] != ' ') { // Solo dibuja caracteres no vacíos
            temp[0] = user[i];
            temp[1] = '\0';
            text(user_col, user_row, (unsigned char *)temp, 0); // Dibuja el carácter
        }
        user_col += 8; // Avanza la posición de la columna
    }

    Nokia5110_DisplayBuffer(); // Asegúrate de actualizar la pantalla
}


// Parpadea la letra seleccionada actualmente
void blinkLetter(void) {
    actualizarContadorPantalla();
    int x, y;
    if(cursor_row == 2 && cursor_col == 6){ // se encuentra en -
        x = 64;
        y = 43;
    }
    else if(cursor_row == 2 && cursor_col == 7){ // se encuentra en +
        x = 72;
        y = 43;
    }
    else{
        x = cursor_col * 8; // Coordenada X de la letra (cada letra ocupa 8px horizontalmente)
        y = cursor_row * 8 + 27; // Coordenada Y (24px desde la parte superior del teclado)
    }
    if(cursor_row == 2 && cursor_col == 8){ // se encuentra en -
        cursor_col = 6;
    }
    if(cursor_row == 2 && cursor_col == 9){ // se encuentra en -
        cursor_col = 7;
    }

    drawKeyboard();
    if(contadorPantalla < 80){
        // Borrar los píxeles de la letra seleccionada
        for (int i = 0; i < 5; i++) { // Alto de la letra
            for (int j = 0; j < 3; j++) { // Ancho de la letra
                clearPixel(x + j, y + i); // Poner en 0 cada píxel
            }
        }
    }

    drawUser(); // el draw user va dentro ya q si lo ponemos afuera habria que refrescar de nuevo la pantalla
    Nokia5110_DisplayBuffer();

}

uint8_t end = 0;

void updateCursor(void){
    //mover a la izquierda
    if ((GPIO_PORTC_DATA_R & SW1) == 0 ) { // se presiona SW1
        while(((GPIO_PORTC_DATA_R & SW1) == 0));
        if(cursor_col != 0){
            cursor_col--; // si no esta en el inicio, mover a la izquierda
        }
    }
    //mover abajo
    if ((GPIO_PORTC_DATA_R & SW2) == 0 ) { // se presiona SW2
        while(((GPIO_PORTC_DATA_R & SW2) == 0));
        if(cursor_row == 2){
            cursor_row = 0; // si esta en la ultima fila, volver a la primera
        }
        else{
            cursor_row++;
        }
    }
    if ((GPIO_PORTC_DATA_R & SW3) == 0 ) { // // se presiona SW3
        while(((GPIO_PORTC_DATA_R & SW3) == 0));
        if(cursor_col != 9){
            cursor_col++;// si no esta en el final, mover a la derecha
        }
    }
    if ((GPIO_PORTC_DATA_R & SW4) == 0 ) {// se presiona SW4
        while(((GPIO_PORTC_DATA_R & SW4) == 0));  // procesar el aceptado
        if(cursor_row == 2 && cursor_col == 6){ // se encuentra en -, se elimina el user anterior
            if(user_count != 0){
                user_count--;
                user[user_count] = ' '; // vaciamos la letra a eliminar
            }
        }
        else if(cursor_row == 2 && cursor_col == 7){ // se encuentra en +, se termina todo
            end = 1;
        }
        else{
            if(user_count != 10){
                user[user_count] = keyboard[cursor_row][cursor_col];
                user_count++;
            }
            else{
                for (int i = 13; i < 26; i++) { // Alto del espacio
                    for (int j = 5; j < 84; j++) { // Ancho del espacio
                        clearPixel(j,i); // Poner en 0 cada píxel
                    }
                }
                text(0, 13,  (unsigned char *)"NAME IS TOO LONG", 0);
                Nokia5110_DisplayBuffer();
                delay(1000);
            }
        }
    }
}

void getUser(char arr[]) {
    end = 0; // Establecemos end en 0
    cursor_row = 0;
    cursor_col = 0;
    user_count = 0; // Reiniciamos parámetros

    // Vaciamos el array para múltiples repeticiones
    for (int j = 0; j < 10; j++) {
        user[j] = ' ';
    }

    while (end == 0) {
        updateCursor();
        blinkLetter();
    }

    // Copiamos el contenido del array user al array arr
    for (int j = 0; j < user_count; j++) {
        arr[j] = user[j];
    }

    // Terminamos la cadena con un carácter nulo
    arr[user_count] = '\0';
}
