#pragma once
#include <windows.h>

// Перелік класичних тетраміно (пентаміно – в наступних версіях гри)
enum struct Type { I, O, T, J, L, S, Z };
// https://en.cppreference.com/w/cpp/language/enum

// Перелік кольорів для тетраміно та інтерфейсу (Week 16 Homework.cpp):
enum struct Color : unsigned char {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    AQUA = 3,
    RED = 4,
    PURPLE = 5,
    YELLOW = 6,
    WHITE = 7,
    GRAY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_AQUA = 11,
    LIGHT_RED = 12,
    LIGHT_PURPLE = 13,
    LIGHT_YELLOW = 14,
    BRIGHT_WHITE = 15
};  // номери кольорів задано явно для зручності запам'ятовування

// Коди зчитування клавіш керування:
enum struct Key : unsigned char {
    // Стрілка вправо: рух праворуч
    RIGHT = 77,      // scan-код dec 77 / hex 0x4D, перед ним 224 / E0
    // Стрілка вліво: рух ліворуч
    LEFT = 75,       // scan-код dec 75 / hex 0x4B, перед ним 224 / E0
    // Стрілка вгору: обертання за годинниковою стрілкою
    ROTATE = 72,     // scan-код dec 72 / hex 48, перед ним 224 / E0
    // Стрілка вниз: м'яке падіння
    SOFT_DROP = 80,  // scan-код dec 80 / hex 50, перед ним 224 / E0 
    // Space / пробіл: жорстке падіння
    HARD_DROP = 32,  // ASCII-код dec 32 / hex 0x20
    // Escape / ESC: пауза гри
    PAUSE = 27,      // ASCII-код dec 27 / hex 0x1B
    // F1: перезапуск гри
    RESTART = 59,    // код _getch() після F1: спочатку 0, потім 59 / 3B
};
// https://www.asciitable.com/  тільки видимі символи та службові коди!
// https://kbdlayout.info/kbdusx/scancodes апаратні коди від клавіатури

// Властивості тетраміно, доступних у версії гри "Тетріс" - "Тетроград":
struct Tetromino {
    Type type;         // тип тетраміно (I, O, T, J, L, S, Z)
    COORD squares[4];  // координати 4 квадратів тетраміно
    Color color;       // колір тетраміно, у класичній грі невипадковий
    COORD offset;      // зсув позиції
};

/**
 * typedef struct _COORD {
 * SHORT X;
 * SHORT Y;
 * } COORD, *PCOORD;
 */

 // Властивості ігрової структури
struct Glass {
    short height;  // висота стакана
    short width;   // ширина стакана
    short startX;  // X-координата початку стакана в консолі
    short startY;  // Y-координата початку стакана в консолі
};
// У цьому проєкті замість терміна "ігрове поле" використано поняття 
// "ігрова структура" та "ігрова мозаїка".

// Структура ігрових досягнень
struct GameStats {
    int score;      // поточний рахунок
    int highScore;  // рекордний рахунок
    short level;    // поточний рівень гри
};