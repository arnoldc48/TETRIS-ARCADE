/********************************************************************************************
 * tetris_v6:
 * PERIFERICOS:
     * Nokia 5110 (PORTA)
         * RST : PA7
         * CE : PA3
         * DC: PA6
         * DIN: PA5
         * SCLK: PA2
     * Switches (PC7 - PC4)
         * SW1 (0x80): Azul, boton para mover a la izquierda
         * SW2 (0x40): Negro, boton para mover abajo
         * SW3 (0x20): Rojo, boton para mover a la derecha
         * SW4 (0x10): Negro, boton para rotar
     * Buzzer (PB4)
     * ESP32 C3:
         * GPIO 1 : PB0 (RX)
         * GPIO 3 : PB1 (TX)
 * NOTAS:
     *
 * PENDIENTES:
     *
********************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "pieces.h"
#include "NOTES.h"
#include "UART_ESP32C3.h"
#include "Nokia5110_Keyboard.h"

#define f_clock 16000000 // 16MHz
#define DC_inicial 50    // 50% Duty Cycle inicial

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

//Variables varias

int n;
int holdforlevel = 0;

// Tablero de juego
int board[BOARD_HEIGHT][BOARD_WIDTH] = {0};
// Pantalla del Nokia5110
//int screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
//Contador Tiempo
uint16_t contadortiempo = 0;
uint32_t guardartiempo;
uint16_t contadorstop = 255;
uint8_t w=0;


// Partitura: Melodía inspirada en Tetris
const uint32_t partitura[] = {
    // Primera sección
    NOTE_E52, NOTE_E5_DUR, NOTE_B42, NOTE_B4_DUR, NOTE_C52, NOTE_C5_DUR, NOTE_D52, NOTE_D5_DUR,
    NOTE_C52, NOTE_C5_DUR, NOTE_B42, NOTE_B4_DUR, NOTE_A42, NOTE_A4_DUR, NOTE_A42, NOTE_A4_DUR,
    NOTE_C52, NOTE_C5_DUR, NOTE_E52, NOTE_E5_DUR, NOTE_D52, NOTE_D5_DUR, NOTE_C52, NOTE_C5_DUR,
    NOTE_B42, NOTE_B4_DUR, NOTE_B42, NOTE_B4_DUR, NOTE_C52, NOTE_C5_DUR, NOTE_D52, NOTE_D5_DUR,
    NOTE_E52, NOTE_E5_DUR, NOTE_C52, NOTE_C5_DUR, NOTE_A42, NOTE_A4_DUR, NOTE_A42, NOTE_A4_DUR, NOTA_SILENC,
    NOTE_NONE,
    // Segunda sección
    NOTE_D52, NOTE_D5_DUR, NOTE_F52, NOTE_F5_DUR, NOTE_A52, NOTE_A4_DUR, NOTE_G52, NOTE_G5_DUR,
    NOTE_F52, NOTE_F5_DUR, NOTE_E52, NOTE_E5_DUR, NOTE_C52, NOTE_C5_DUR, NOTE_E52, NOTE_E5_DUR,
    NOTE_D52, NOTE_D5_DUR, NOTE_C52, NOTE_C5_DUR, NOTE_B42, NOTE_B4_DUR, NOTE_B42, NOTE_B4_DUR,
    NOTE_C52, NOTE_C5_DUR, NOTE_D52, NOTE_D5_DUR, NOTE_E52, NOTE_E5_DUR, NOTE_C52, NOTE_C5_DUR,
    NOTE_A42, NOTE_A4_DUR, NOTE_A42, NOTE_A4_DUR, NOTA_SILENC,NOTE_NONE,

    // Tercera sección
    NOTE_E52, NOTE_E5_DUR, NOTE_B42, NOTE_B4_DUR, NOTE_C52, NOTE_C5_DUR, NOTE_D52, NOTE_D5_DUR,
    NOTE_C52, NOTE_C5_DUR, NOTE_B42, NOTE_B4_DUR, NOTE_A42, NOTE_A4_DUR, NOTE_A42, NOTE_A4_DUR,
    NOTE_C52, NOTE_C5_DUR, NOTE_E52, NOTE_E5_DUR, NOTE_D52, NOTE_D5_DUR, NOTE_C52, NOTE_C5_DUR,
    NOTE_B42, NOTE_B4_DUR, NOTE_B42, NOTE_B4_DUR, NOTE_C52, NOTE_C5_DUR, NOTE_D52, NOTE_D5_DUR,
    NOTE_E52, NOTE_E5_DUR, NOTE_C52, NOTE_C5_DUR, NOTE_A42, NOTE_A4_DUR, NOTE_A42, NOTE_A4_DUR, NOTA_SILENC,
    NOTE_NONE,
    // Cuarta sección
    NOTE_D52, NOTE_D5_DUR, NOTE_F52, NOTE_F5_DUR, NOTE_A52, NOTE_A4_DUR, NOTE_G52, NOTE_G5_DUR,
    NOTE_F52, NOTE_F5_DUR, NOTE_E52, NOTE_E5_DUR, NOTE_C52, NOTE_C5_DUR, NOTE_E52, NOTE_E5_DUR,
    NOTE_D52, NOTE_D5_DUR, NOTE_C52, NOTE_C5_DUR, NOTE_B42, NOTE_B4_DUR, NOTE_B42, NOTE_B4_DUR,
    NOTE_C52, NOTE_C5_DUR, NOTE_D52, NOTE_D5_DUR, NOTE_E52, NOTE_E5_DUR, NOTE_C52, NOTE_C5_DUR,
    NOTE_A42, NOTE_A4_DUR, NOTE_A42, NOTE_A4_DUR, NOTA_SILENC,NOTE_NONE,

    // Quinta sección
    NOTE_B42, NOTE_B4_DUR, NOTE_C52, NOTE_C5_DUR, NOTE_D52, NOTE_D5_DUR, NOTE_E52, NOTE_E5_DUR,
    NOTE_C52, NOTE_C5_DUR, NOTE_A42, NOTE_A4_DUR, NOTE_A42, NOTE_A4_DUR, NOTA_SILENC
};


// Variables globales
volatile uint32_t notaActual = 0;  // Índice de la nota actual en la partitura
volatile uint32_t tiempoNota = 0;  // Tiempo restante de la nota actual
volatile uint8_t enSilencio = 0;   // Indica si estamos en silencio

void configuraPiano(void) {
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;       // Habilitar reloj para Puerto B
    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1)); // Esperar activación del reloj

    GPIO_PORTB_AFSEL_R |= 0x04;                // Seleccionar función alternativa en PB2
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0x00000F00) | 0x00000700; // PB2 como T3CCP0
    GPIO_PORTB_DEN_R |= 0x04;                  // Habilitar función digital en PB2
    GPIO_PORTB_DIR_R |= 0x04;                  // Configurar PB2 como salida

    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3; // Activar reloj para TIMER3
    while (!(SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R3)); // Esperar activación del reloj

    TIMER3_CTL_R &= ~TIMER_CTL_TAEN;           // Deshabilitar TIMER3A para configurar
    TIMER3_CFG_R = 0x4;                        // Configurar modo de 16 bits
    TIMER3_TAMR_R = 0x0A;                      // Configurar modo PWM periódico
    TIMER3_CTL_R |= TIMER_CTL_TAPWML;          // Configurar PWM invertido
}

void generaNota(uint8_t nota) {
    if (nota == NOTA_SILENC || tablaModulos[nota] == 0) {
        TIMER3_CTL_R &= ~TIMER_CTL_TAEN; // Detener temporizador
        return;
    }

    uint32_t modulo = tablaModulos[nota];
    TIMER3_TAILR_R = modulo - 1;            // Configurar el periodo
    TIMER3_TAMATCHR_R = (modulo / 2) - 1;  // Ciclo de trabajo al 50%
    TIMER3_CTL_R |= TIMER_CTL_TAEN;        // Activar el temporizador
}


void configuraSysTick(void) {
    //Inhabilitamos el módulo SysTick
    NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
    NVIC_ST_RELOAD_R = (NVIC_ST_RELOAD_R&0xFF000000)|159999;
    // Iniciamos el contador con cero (escribiendo cualquier valor)
    NVIC_ST_CURRENT_R &= ~(0x00FFFFFF);
    // Habilitamos el módulo SysTick
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_ENABLE; // Habilitar SysTick
}

void SysTick_Handler(void) {
    if (notaActual >= sizeof(partitura) / sizeof(partitura[0])) {
        // Fin de la partitura, apagar buzzer y deshabilitar SysTick
        TIMER3_CTL_R &= ~TIMER_CTL_TAEN;  // Detener temporizador del buzzer
        GPIO_PORTB_DATA_R &= ~0x04;       // Asegurarse de que el pin PB2 esté en bajo
        GPIO_PORTB_DIR_R &= ~0x04;       // Configurar PB2 como entrada para evitar cualquier señal residual
        NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;  // Deshabilitar SysTick
        return;  // Salir del manejador
    }

    if (tiempoNota == 0) {
        if (enSilencio) {
            enSilencio = 0; // Finaliza el silencio
            notaActual += 2; // Avanzar al siguiente par (nota y duración)
            if (notaActual < sizeof(partitura) / sizeof(partitura[0])) {
                generaNota(partitura[notaActual]); // Generar nueva nota
                tiempoNota = partitura[notaActual + 1]; // Duración completa de la nota
            }
        } else {
            generaNota(NOTA_SILENC); // Generar silencio
            enSilencio = 1;
            tiempoNota = partitura[notaActual + 1]; // Duración completa de silencio
        }
    } else {
        tiempoNota--; // Reducir tiempo restante
    }
}
void enable_irq(void) {
    __asm(" cpsie i \n");
}

void disable_irq(void) {
    __asm(" cpsid i \n");
}



// Posición y estado de la pieza actual
typedef struct {
    int x;
    int y;
    int type;
    int rotation;
} TetrisPiece;

TetrisPiece currentPiece;


void Config_Puerto_C(void) {
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2; // Activa el puerto C
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R2) == 0); // Espera a que se active
    GPIO_PORTC_DIR_R &= ~0xF0; // Configura PC7, PC6, PC5, PC4 como entrada
    GPIO_PORTC_DEN_R |= 0xF0; // Habilita PC7, PC6, PC5, PC4 como digital
    GPIO_PORTC_PUR_R |= 0xF0; // Activa el pull-up en PC7, PC6, PC5, PC4
}

// Función para inicializar el microcontrolador y la pantalla
void InitSystem(void) {
    Nokia5110_Init();
    Nokia5110_Clear();
    Nokia5110_SetCursor(0, 0);
}


void ConfiguraTimer_1ms(void) {
    /*Inhabilitamos el módulo SysTick*/
    NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
    /* TiempoDeseado=1ms; FreqCPU=16MHZ
    * valor que debe ir en el registro RELOAD=X
    * X=(TiempoDeseado)*(FreqCPU) -1
    * X= [[1*10^(-3)] * [16*10^(6)]] -1 =15999
    *15999 en hexadecimal es 00003E7F*/
    NVIC_ST_RELOAD_R = (NVIC_ST_RELOAD_R&0xFF000000)|0x00003E7F;
    // Iniciamos el contador con cero (escribiendo cualquier valor)
    NVIC_ST_CURRENT_R &= ~(0x00FFFFFF);
    // Habilitamos el módulo SysTick
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE; // Habilitar SysTick
}

// Función para agregar una pieza al tablero
void addNewPiece(void) {
    currentPiece.x = 0; // Posición inicial en el centro
    currentPiece.y = 3; // Parte superior del tablero
    //currentPiece.type = 0; //Para probar
    currentPiece.type = rand() % 7; // Tipo de pieza aleatorio (0 a 6)
    currentPiece.rotation = 0; // Rotación inicial

    // Agrega la pieza al tablero
    int col,row;
    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            if (pieces[currentPiece.type][currentPiece.rotation][row][col] == 1) {
                int boardX = currentPiece.x + col;
                int boardY = currentPiece.y + row;
                if (boardX >= 0 && boardX < BOARD_WIDTH && boardY >= 0 && boardY < BOARD_HEIGHT) {
                    board[boardY][boardX] = 1;
                }
            }
        }
    }
}

// Función para eliminar una pieza del tablero
void removePiece(void) {
    int col, row;
    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            // Si la celda de la pieza actual está ocupada (valor 1), la elimina del tablero
            if (pieces[currentPiece.type][currentPiece.rotation][row][col] == 1) {
                int boardX = currentPiece.x + col;
                int boardY = currentPiece.y + row;
                // Asegura que las coordenadas estén dentro de los límites del tablero
                if (boardX >= 0 && boardX < BOARD_WIDTH && boardY >= 0 && boardY < BOARD_HEIGHT) {
                    board[boardY][boardX] = 0;  // Borra la celda en el tablero
                }
            }
        }
    }
}

//Función que añade una pieza al tablero
void addPieceToBoard(void) {
    int col, row;
    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            if (pieces[currentPiece.type][currentPiece.rotation][row][col] == 1) {
                int boardX = currentPiece.x + col;
                int boardY = currentPiece.y + row;
                if (boardX >= 0 && boardX < BOARD_WIDTH && boardY >= 0 && boardY < BOARD_HEIGHT) {
                    board[boardY][boardX] = 1; // Coloca la pieza en el tablero
                }
            }
        }
    }
}

//Función que verifica colisiones

int verifyCollisions(TetrisPiece piece) {
    int col, row;
    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            if (pieces[piece.type][piece.rotation][row][col] == 1) {
                int boardX = piece.x + col;
                int boardY = piece.y + row;
                if (boardY<0 || boardX < 0 || boardX >= BOARD_WIDTH || boardY >= BOARD_HEIGHT || (boardY >= 0 && board[boardY][boardX] == 1)) {
                    return 0; // Posición inválida
                }
            }
        }
    }
    return 1; // Posición válida
}

// Función para dibujar un píxel de 4x4 en la pantalla
void DrawPixelAsBlock(int x, int y) {
    int offsetY, offsetX;
    for (offsetY = 0; offsetY < 4; offsetY++) {
        for (offsetX = 0; offsetX < 4; offsetX++) {
            if ((x + offsetX) < SCREEN_HEIGHT && (y + offsetY) < SCREEN_WIDTH) {
                setPixel(x + offsetX, y + offsetY);
            }
        }
    }
}


int score;//////////////////////SCOOOOOOOOOREEEEEE///////////////////////////

void RemoveLine() {
    int row, col, complete,k;
    for (row = 0; row < BOARD_WIDTH; row++) {
        complete = 1;  // Asumimos que la fila está completa
        for (col = 0; col < BOARD_HEIGHT; col++) {
            if (board[col][row] == 0) {  // Si hay una celda vacía, la fila no está completa
                complete = 0;
                break;
            }
        }

        if (complete) {

            // Borrar la fila que contiene a la linea completa (se convierte en una nueva fila vacía)
            for (col = 0; col < BOARD_HEIGHT; col++) {
                board[col][row] = 0;
            }
            // Desplazar todas las filas superiores hacia abajo
            for (k = row; k > 1; k--){
                for (col = 0; col < BOARD_HEIGHT; col++) {
                    board[col][k] = board[col][k-1];
                }
            }

            score += 100;  // Sumar puntos al marcador
            //updateScoreDisplay();  // Actualizar el marcador en la pantalla
            if(score == 500) contadorstop = 200, holdforlevel = 1;
            if(score == 1000) contadorstop = 127, holdforlevel = 1;
            if(score == 1500) contadorstop = 55, holdforlevel = 1;

            while(holdforlevel){
                if(score >= 500 && score < 1000){
                    clearBuffer();
                    Nokia5110_DisplayBuffer();
                    text(0, 15,  (unsigned char *)"LEVEL 1 COMPLETED", 0);
                    Nokia5110_DisplayBuffer();
                    for(n = 0; n < 3600000; n++); //retardo
                    holdforlevel = 0;

                }
                else if(score >= 1000 && score < 1500){
                    clearBuffer();
                    Nokia5110_DisplayBuffer();
                    text(0, 15,  (unsigned char *)"LEVEL 2 COMPLETED", 0);
                    Nokia5110_DisplayBuffer();
                    for(n = 0; n < 3600000; n++); //retardo
                    holdforlevel = 0;

                }
                else if(score >= 1500){
                    clearBuffer();
                    Nokia5110_DisplayBuffer();
                    text(0, 15,  (unsigned char *)"LEVEL 3 COMPLETED", 0);
                    text(0, 22,  (unsigned char *)"INFINITE MODE", 0);
                    Nokia5110_DisplayBuffer();
                    for(n = 0; n < 3600000; n++); //retardo
                    holdforlevel = 0;

                }
            }

        }
    }
}


void intToStr(int num, unsigned char *str) {
    int i = 0;
    int j;
    int temp;

    // Manejar el caso cuando el número es 0
    if (num == 0) {
        str[i++] = '0';
    } else {
        // Convierte el número a una cadena
        while (num > 0) {
            str[i++] = (num % 10) + '0';  // Extrae el dígito y lo convierte a carácter
            num /= 10;
        }
    }

    str[i] = '\0';  // Añade el terminador de cadena

    // Invierte la cadena para obtener el número en el orden correcto
    for (j = 0; j < i / 2; j++) {
        temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}


// Función para dibujar el tablero en la pantalla
void DrawBoard(void) {
    Nokia5110_ClearBuffer();
    int x,y;
    // Dibuja el tablero
    for (y = 0; y < BOARD_HEIGHT; y++) {
        for (x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x] == 1) {
                DrawPixelAsBlock(x * 4, y * 4); // Dibuja cada píxel como un bloque de 4x4
            }
        }
    }
    unsigned char scoreStr[7];
    intToStr(score, scoreStr);

    text(16, 43,  (unsigned char *)"SCORE: ", 0);
    text(40, 43,  scoreStr, 0);
    Nokia5110_DisplayBuffer();
}


// Función para actualizar el contador de tiempo
void actualizarContador() {
//preguntamos si paso 1ms
    if((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT)) {
        //cada vez que pase 1ms, el contador de tiempo se incrementa
        contadortiempo++;
    }
}

char name[11] = {' '};

void uploadScore(void){
    getUser(name);
    char score_char[10];
    IntToCharArray(score, score_char);
    UART1_Transmite_Cadena("USER:");
    UART1_Transmite_Cadena(name);
    UART1_Transmite_Cadena(";");
    UART1_Transmite_Cadena("SCORE:");
    UART1_Transmite_Cadena(score_char);      // Transmite el puntaje como cadena completa
    UART1_Transmite_Cadena(";");
}



// Función principal
int main(void) {
     configuraPiano();
     disable_irq();
     configuraSysTick();
     enable_irq();
    int row,col;

    InitSystem();

    Config_Puerto_C();
    ConfiguraTimer_1ms();
    UART1_Init();
    while (1) {
        int game = 0;

        clearBuffer();
        Nokia5110_DisplayBuffer();
        int hold = 1;
        while(hold){
            text(0, 15,  (unsigned char *)"PRESS R", 0);
            text(0, 22,  (unsigned char *)"TO START", 0);
            Nokia5110_DisplayBuffer();
            if((GPIO_PORTC_DATA_R & SW4) == 0){
                while(((GPIO_PORTC_DATA_R & SW4) == 0));
                game = 1;
                // Inicializa el generador de números aleatorios
                clearBuffer();
                Nokia5110_DisplayBuffer();
                srand(12345); // Puedes usar una semilla diferente para resultados variados

                // Agrega una nueva pieza al tablero
                addNewPiece();
                DrawBoard();
                delay(255);
                hold = 0;
            }
        }
        while(game){ // LOOP DEL JUEGO
            //inicializa contador
            actualizarContador();


            if(contadortiempo == contadorstop){
                removePiece();     // Elimina la pieza actual del tablero
                currentPiece.x++;  // Mueve la pieza hacia abajo

                // Verifica si la nueva posición es válida
                if (!verifyCollisions(currentPiece)) {
                    currentPiece.x--;  // Si no es válida, restaura la posición anterior

                    // ACA SE VERIFICA SI SE PIERDE EL JUEGO
                        for (col = 0; col < BOARD_HEIGHT; col++) {
                            if (board[col][0] == 1){ // si se ha alcanzado el limite superior
                                for (row = 0; row < BOARD_WIDTH; row++){
                                    for (col = 0; col < BOARD_HEIGHT; col++) {
                                        board[col][row] = 0; // SE VACIA EL BOARD
                                    }
                                }
                                game = 0;//se apaga el juego
                                DrawBoard();
                                hold = 1;
                                while(hold){
                                    text(0, 15,  (unsigned char *)"SAVE SCORE?", 0);
                                    text(0, 22,  (unsigned char *)"<= YES    => NO", 0);
                                    Nokia5110_DisplayBuffer();
                                    if((GPIO_PORTC_DATA_R & SW1) == 0){
                                        while((GPIO_PORTC_DATA_R & SW1) == 0);
                                        clearBuffer();
                                        Nokia5110_DisplayBuffer();
                                        uploadScore();
                                        clearBuffer();
                                        text(0, 15,  (unsigned char *)"SCORE SAVED", 0);
                                        Nokia5110_DisplayBuffer();
                                        for(n = 0; n < 3600000; n++); //retardo
                                        score = 0;
                                        contadorstop = 255;
                                        hold = 0;
                                    }
                                    else if((GPIO_PORTC_DATA_R & SW3) == 0){
                                        while((GPIO_PORTC_DATA_R & SW3) == 0);
                                        clearBuffer();
                                        Nokia5110_DisplayBuffer();
                                        text(0, 15,  (unsigned char *)"SCORE NOT SAVED", 0);
                                        Nokia5110_DisplayBuffer();
                                        for(n = 0; n < 3600000; n++); //retardo
                                        score = 0;
                                        contadorstop = 255;
                                        hold = 0;
                                    }
                                }
                            }
                        }
                    if(game != 0){ // si no termina el juego
                        addPieceToBoard(); // Coloca la pieza permanentemente en el tablero
                        RemoveLine();  // Limpia líneas completas y actualiza el puntaje
                        addNewPiece();     // Genera una nueva pieza en la parte superior
                    }
                } else {
                    addPieceToBoard(); // Si es válida, coloca la pieza en la nueva posición
                }
                //Nokia5110_SetCursor(0, 0);
                DrawBoard(); // Actualiza la pantalla para mostrar el nuevo estado del tablero
            }
            else if(contadortiempo > contadorstop){
                contadortiempo = 0;
            }
            // ARRIBA
            if ((GPIO_PORTC_DATA_R & SW1) == 0) {
                while(((GPIO_PORTC_DATA_R & SW1) == 0));
                //delay(50);
                removePiece();     // Elimina la pieza actual del tablero
                currentPiece.y++;  // Mueve la pieza hacia izquierda
                // Verifica si la nueva posición es válida
                if (!verifyCollisions(currentPiece)) {
                    currentPiece.y--;  // Si no es válida, restaura la posición anterior
                }
                addPieceToBoard();
                DrawBoard(); // Actualiza la pantalla para mostrar el nuevo estado del tablero
            }
            // ABAJO
            if ((GPIO_PORTC_DATA_R & SW2) == 0 ) {
                while(((GPIO_PORTC_DATA_R & SW2) == 0));
                //delay(50); // Retardo antirrebote
                removePiece();     // Elimina la pieza actual del tablero

                currentPiece.x++;  // Mueve la pieza hacia la derecha

                // Verifica si la nueva posición es válida
                if (!verifyCollisions(currentPiece)) {
                    currentPiece.x--;  // Si no es válida, restaura la posición anterior
                }
                addPieceToBoard();
                DrawBoard(); // Actualiza la pantalla para mostrar el nuevo estado del tablero
            }
            // IZQUIERDA
            if ((GPIO_PORTC_DATA_R & SW3) == 0 ) { // SW2 presionado y música encendida
                while(((GPIO_PORTC_DATA_R & SW3) == 0));
                //delay(50); // Retardo antirrebote
                removePiece();     // Elimina la pieza actual del tablero

                currentPiece.y--;  // Mueve la pieza hacia la derecha

                // Verifica si la nueva posición es válida
                if (!verifyCollisions(currentPiece)) {
                    currentPiece.y++;  // Si no es válida, restaura la posición anterior
                }
                addPieceToBoard();
                DrawBoard(); // Actualiza la pantalla para mostrar el nuevo estado del tablero
            }
            // ROTAR
            if ((GPIO_PORTC_DATA_R & SW4) == 0 ) {
                while(((GPIO_PORTC_DATA_R & SW4) == 0));
                //delay(50); // Retardo antirrebote
                removePiece();     // Elimina la pieza actual del tablero

                currentPiece.rotation++;  // Realiza una rotación de 90 grados baby
                if(currentPiece.rotation == 4){
                    currentPiece.rotation = currentPiece.rotation - 4;
                }
                // Verifica si la nueva posición es válida
                if (!verifyCollisions(currentPiece)) {
                    currentPiece.rotation--;  // Si no es válida, restaura la posición anterior
                    if(currentPiece.rotation < 0){
                        currentPiece.rotation = currentPiece.rotation + 5;
                    }
                }
                addPieceToBoard();
                DrawBoard(); // Actualiza la pantalla para mostrar el nuevo estado del tablero
            }
        }
    }
}
