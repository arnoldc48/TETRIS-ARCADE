#define FCPU 16000000UL // Frecuencia del reloj del sistema

// Frecuencias de las notas musicales
#define NOTE_E5  659
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_A4  440
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_NONE 0

// Identificadores de las notas musicales
#define NOTA_SILENC 0
#define NOTE_E52    1
#define NOTE_B42    2
#define NOTE_C52    3
#define NOTE_D52    4
#define NOTE_A42    5
#define NOTE_F52    6
#define NOTE_G52    7
#define NOTE_A52    8


// Duraciones de las notas (en milisegundos)
#define NOTE_E5_DUR 8
#define NOTE_B4_DUR 8
#define NOTE_C5_DUR 8
#define NOTE_D5_DUR 8
#define NOTE_A4_DUR 8
#define NOTE_A5_DUR 8
#define NOTE_F5_DUR 8
#define NOTE_G5_DUR 8

const uint32_t tablaModulos[] = {
    0,                    // Silencio
    FCPU / NOTE_E5,       // E5
    FCPU / NOTE_B4,       // B4
    FCPU / NOTE_C5,       // C5
    FCPU / NOTE_D5,       // D5
    FCPU / NOTE_A4,       // A4
    FCPU / NOTE_F5,       // F5
    FCPU / NOTE_G5        // G5
};
