#ifndef MELODIA_H
#define MELODIA_H

#include "NOTES.h"

// Declaraciones de las melodías y tempos
extern int musica[];
extern int musica_tempo[];
extern int tetris_musica[];
extern int tetris_musica_tempo[];
extern int lose_musica[];
extern int lose_musica_tempo[];
extern int win_musica[];
extern int win_musica_tempo[];

// Tamaños de las melodías
#define WIN_MELODY_SIZE 16 //cantidad de notas
#define MELODY_SIZE_EXISTENTE 33 //cantidad de notas
#define TETRIS_MELODY_SIZE 93 //cantidad de notas
#define LOSE_MELODY_SIZE 8 //cantidad de notas

#endif // MELODIA_H
