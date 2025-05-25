#pragma once
#include "tetrograd_config.h"
#include "tetrograd_core.h"
#include <string>

using std::string;

// ПРОТОТИПИ ФУНКЦІЙ

// 1. Налаштування консолі та ініціалізація генератора випадкових чисел
void util_SetConsoleOptions(HANDLE h);

// 2. Отримання розмірів вікна консолі для перевірки мінімальних вимог
void util_GetConsoleSize(HANDLE h, short& outWidth, short& outHeight);

// 3. Створення двовимірного масиву кольорів ігрової мозаїки
Color** v4_CreateMosaic(const Glass& cup);

// 4. Створення тетраміно із заданим типом, кольором та зсувом позиції
Tetromino v4_CreateTetromino(Type type, const Glass& cup);

// 5. Завантаження рекордного рахунку з файлу
int util_LoadHighScore();

// 6. Ініціалізація ігрової статистики для обробника сигналів консолі
void util_InitHandlerStats(GameStats* stats);

// 7. Обробник сигналів консолі Windows (Ctrl+C, закриття вікна тощо)
BOOL WINAPI util_ConsoleHandler(DWORD signal);

// 8. Відображення текстової довідки про клавіші керування грою
void v4_DrawControls(HANDLE h, COORD ctrlPos);

// 9. Малювання в консолі ASCII-зображення ігрового "стакана"
void v4_DrawGlass(HANDLE h, const Glass& cup);

// 10. Малювання кольорової мозаїки гри
void v4_DrawMosaic(HANDLE h, Color** mosaic, const Glass& cup);

// 11. Малювання наступного тетраміно в заданій області
void v4_DrawNextTetromino(HANDLE h, const Tetromino& next, COORD nextPos);

// 12. Відображення статистики гри в консолі (рахунок, рівень, рекорд)
void v4_DrawStatistics(HANDLE h, const GameStats& stats, COORD statsPos);

// 13. Виведення заданого тексту у вказаній позиції з заданим кольором
void util_ShowText(HANDLE h, short x, short y, const string& text,
    Color color);

// 14. Ефект стирання тетраміно пробілами для оновлення позиції
void v4_EraseTetromino(HANDLE h, const Tetromino& tm);

// 15. Перевірка можливості руху тетраміно без виходу за межі чи колізій
bool v4_CanMove(const Tetromino& tm, COORD delta, Color** mosaic,
    const Glass& cup);

// 16. Обертання тетраміно на 90 градусів із механізмом "wall kicks"
void v4_RotateTetromino(Tetromino& tm, Color** mosaic, const Glass& cup);

// 17. Додавання тетраміно до ігрової мозаїки, його знерухомлення
void v4_FixTetromino(const Tetromino& tm, Color** mosaic, const Glass& cup);

// 18. Виявлення та очищення заповнених ліній, оновлення статистики
void v4_CheckLines(HANDLE h, Color** mosaic, const Glass& cup,
    GameStats& stats, COORD bonusPos);

// 19. Перевірка, чи не настав кінець гри: заповнено верхній ряд стакана
// або неможливо розмістити нове тетраміно
bool v4_CheckGameOver(Color** mosaic, const Glass& cup,
    const Tetromino& current);

// 20. Малювання поточного тетраміно в його поточній позиції
void v4_DrawCurrentTetromino(HANDLE h, const Tetromino& tm);

// 21. Виведення тетраміно в заданій позиції, ігноруючи його зміщення
void v4_ShowTetrominoAt(HANDLE h, const Tetromino& tm, COORD position);

// 22. Збереження рекордного рахунку у файл
void util_SaveHighScore(const GameStats& stats);

// 23. Виведення повідомлення про завершення гри з можливістю рестарту
void v4_DrawGameOver(HANDLE h, const Glass& cup, const GameStats& stats,
    bool& restart);

// 24. Очищення динамічної пам'яті, виділеної для ігрової мозаїки
void v4_DeleteMosaic(Color** mosaic, short height);

// 25. Відновлення початкових налаштувань консолі наприкінці гри
void util_RestoreConsoleState(HANDLE h);

/**
 * Порядок прототипів точно відображає послідовність викликів у main():
 *
 * Ініціалізація: util_SetConsoleOptions, util_GetConsoleSize,
 * v4_CreateMosaic, v4_CreateTetromino, util_LoadHighScore,
 * util_InitHandlerStats, util_ConsoleHandler, v4_DrawControls.
 *
 * Малювання початкового стану: v4_DrawGlass, v4_DrawMosaic,
 * v4_DrawNextTetromino, v4_DrawStatistics.
 *
 * Ігровий цикл: util_ShowText, v4_EraseTetromino, v4_CanMove,
 * v4_RotateTetromino, v4_FixTetromino, v4_CheckLines,
 * v4_CheckGameOver, v4_DrawCurrentTetromino, v4_ShowTetrominoAt.
 *
 * Завершення гри: util_SaveHighScore, v4_DrawGameOver,
 * v4_DeleteMosaic, util_RestoreConsoleState.
 */