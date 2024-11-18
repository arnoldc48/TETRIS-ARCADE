#include "melodia.h"

// Melodía existente
int musica[] = {
    NOTE_D4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, NOTE_F4,
    NOTE_D4, NOTE_D4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_D4, NOTE_C4, NOTE_D4,
    NOTE_E4, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_F4,
    NOTE_D4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_G4,
};
int musica_tempo[] = { 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12 };

// Melodía Tetris
int tetris_musica[] = {
    NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_A4,
    NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5,
    NOTE_E5, NOTE_C5, NOTE_A4, NOTE_A4, NOTE_NONE,

    NOTE_D5, NOTE_F5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_C5, NOTE_E5,
    NOTE_D5, NOTE_C5, NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_A4, NOTE_A4, NOTE_NONE,

    NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_A4,
    NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5,
    NOTE_E5, NOTE_C5, NOTE_A4, NOTE_A4, NOTE_NONE,

    NOTE_D5, NOTE_F5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_C5, NOTE_E5,
    NOTE_D5, NOTE_C5, NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_A4, NOTE_A4,

    NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_A4, NOTE_A4
};
int tetris_musica_tempo[] = {
    8, 8, 8, 8, 8, 8, 4, 4,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 4, 4, 4,

    8, 8, 4, 8, 8, 8, 8, 8,
    8, 8, 4, 8, 8, 8, 8, 8, 4, 4, 4,

    8, 8, 8, 8, 8, 8, 4, 4,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 4, 4, 4,

    8, 8, 4, 8, 8, 8, 8, 8,
    8, 8, 4, 8, 8, 8, 8, 8, 4, 4,

    // Tiempos para el cierre
    8, 8, 8, 8, 8, 4, 4
};
// Melodía de "jugador perdió"
int lose_musica[] = { NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_G3 };
int lose_musica_tempo[] = { 12, 12, 12, 12, 12, 12, 12, 12 };

// Melodía de victoria

int win_musica[] = {
    NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, // Ascenso de tonos
    NOTE_C5, NOTE_G4, NOTE_E4, NOTE_C4, // Descenso de tonos
    NOTE_G4, NOTE_C5, NOTE_E5, NOTE_G5, // Secuencia de celebración
    NOTE_G5, NOTE_E5, NOTE_C5, NOTE_G4, // Finalización
};

int win_musica_tempo[] = {
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12
};