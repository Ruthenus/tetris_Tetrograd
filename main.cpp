/*
                               ПРАКТИЧНИЙ ПРОЄКТ
               з дисципліни "Програмування з використанням мови C++"
                              (процедурна частина)


                            Двовимірна гра "ТЕТРІС"


                                               Виконав студент гр. СПР411
                                            Комп'ютерної Академії IT STEP
                                                        Качуровський Р.Р.


                                  Одеса 2025
*/

#include "tetrograd_config.h"
#include "tetrograd_core.h"
#include "tetrograd_functions.h"

#include <iostream>
#include <conio.h>  // _getch(), _kbhit() — для роботи з клавіатурою
#include <string>   // to_string, string()
#include <cstdlib>  // srand(), rand()
#include <ctime>    // time()


using namespace std;


/**
 * Основна функція гри "Тетріс" - "Тетроград". Ініціалізує консоль,
 * перевіряє її розмір, створює ігрові об'єкти (стакан, мозаїку,
 * статистику, тетраміно), виконує основний ігровий цикл із обробкою
 * введення, руху тетраміно, паузи, перезапуску та завершення гри.
 * Уся логіка, включаючи паузу та рестарт, реалізована в межах цієї
 * функції без використання додаткових функцій для ігрового циклу.
 * Return: int. Повертає 0 у разі успішного виконання, 1 у разі помилок.
 */
int main()
{
    // Отримання дескриптора консолі для виведення графіки та тексту
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE) {
        util_ShowText(h, 0, 1, "Помилка отримання дескриптора консолі!\n",
            Color::WHITE);
        return 1;
    }

    // Налаштування консолі, ініціалізація генератора випадкових чисел
    util_SetConsoleOptions(h);

    // Перевірка розмірів консолі
    short consoleW, consoleH;
    util_GetConsoleSize(h, consoleW, consoleH);
    // cout << consoleW << "\n";
    // cout << consoleH;
    if (consoleW < 100 || consoleH < 30) {
        util_RestoreConsoleState(h);
        // cout << consoleW << "\n";
        // cout << consoleH;
        cout << "\n    Recommended parameters for Your Console\n\n"
            << "    Startup\n"
            << "        Launch size\n"
            << "            Columns       150\n"
            << "            Rows          50\n"
            << "        Launch parameters\n"
            << "            Launch mode          Maximized\n"
            << "            Launch position      Let Windows decide\n"
            << "            Center on launch     On\n\n"
            << "    Defaults > Appearance\n"
            << "        Color scheme             Campbell\n"
            << "        Font face                Lucida Console\n"
            << "        Font size                18\n"
            << "        Font weight              Normal\n"
            << "        Retro terminal effects   On\n";
        Sleep(120000);  // 2 хвилини, щоб переписати, вистачить?
        return 1;
    }

    // Ініціалізація ігрової структури - стакана (див. документацію)
    Glass cup = { 20, 10, static_cast<short>(consoleW / 2 - 10), 5 };

    // Створення ігрової мозаїки - динамічного масиву кольорів
    Color** mosaic = v4_CreateMosaic(cup);
    if (!mosaic) {
        util_ShowText(h, 0, 1, "Помилка створення мозаїки!", Color::RED);
        Sleep(3000);  // затримка 3 с для відображення повідомлення
        util_RestoreConsoleState(h);
        return 1;
    }

    // Ініціалізація статистики гри (рахунок, рекорд, рівень)
    GameStats stats = { 0, util_LoadHighScore(), 1 };
    if (stats.highScore == -1) {
        util_ShowText(h, 0, 1,
            "Помилка завантаження рекорду! Встановлено 0.\n", Color::WHITE);
        stats.highScore = 0;
    }

    // Створення початкових тетраміно - що падає та яке буде падати
    Tetromino current = v4_CreateTetromino(static_cast<Type>(rand() % 7), cup);
    Tetromino next = v4_CreateTetromino(static_cast<Type>(rand() % 7), cup);

    // Ініціалізація швидкості падіння та прапорця завершення гри
    DWORD tick = TetrisConstants::INITIAL_TICK;  // інтервал падіння
    bool gameOver = false;

    // Ініціалізація змінної activeStats у файлі tetrograd_utils.cpp
    util_InitHandlerStats(&stats);  // до встановлення обробника!

    // Установлення обробника сигналів консолі
    if (!SetConsoleCtrlHandler(util_ConsoleHandler, TRUE)) {
        v4_DeleteMosaic(mosaic, cup.height);
        util_ShowText(h, 0, 1, "Помилка встановлення обробника сигналів!",
            Color::RED);
        Sleep(3000);
        util_RestoreConsoleState(h);
        return 1;
    }

    // Ініціалізація таймера для контролю швидкості падіння
    clock_t last = clock();  // https://en.cppreference.com/w/c/chrono/clock

    // Позиція наступного тетраміно
    COORD nextPos = { static_cast<SHORT>(cup.startX + cup.width +
        TetrisConstants::MARGIN_RIGHT), TetrisConstants::NEXT_Y };
    // Позиція статистики гри
    COORD statsPos = { static_cast<SHORT>(cup.startX + cup.width +
        TetrisConstants::MARGIN_RIGHT), TetrisConstants::STATS_Y };
    // Позиція бонусного повідомлення
    COORD bonusPos = { static_cast<SHORT>(cup.startX + cup.width +
        TetrisConstants::MARGIN_RIGHT), TetrisConstants::TETRIS_Y };

    // Відображення довідки про керування
    v4_DrawControls(h, { static_cast<SHORT>(cup.startX + cup.width +
        TetrisConstants::MARGIN_RIGHT), TetrisConstants::CONTROLS_Y });

    // ОСНОВНИЙ ІГРОВИЙ ЦИКЛ

restart:  // мітка для перезапуску ретро-гри
    // https://en.cppreference.com/w/cpp/language/statements#Labels
    do {  // програмування в стилі ретро, чи не так?

        bool restart = false;  // прапорець для рестарту гри

        // Обробка введення з клавіатури
        if (_kbhit()) {
            int key = _getch();
            if (key == 224 || key == 0) key = _getch();

            // Стираємо поточне тетраміно перед рухом
            v4_EraseTetromino(h, current);
            // Перевіряємо можливість переміщення та обертання
            switch (static_cast<Key>(key)) {
            case Key::LEFT:
                if (v4_CanMove(current, { -1, 0 }, mosaic, cup))
                    current.offset.X--;
                break;
            case Key::RIGHT:
                if (v4_CanMove(current, { 1, 0 }, mosaic, cup))
                    current.offset.X++;
                break;
            case Key::SOFT_DROP:
                if (v4_CanMove(current, { 0, 1 }, mosaic, cup))
                    current.offset.Y++;
                break;
            case Key::ROTATE: // клавіша вгору для обертання по часовій
                v4_RotateTetromino(current, mosaic, cup);
                break;
            case Key::HARD_DROP:
                while (v4_CanMove(current, { 0, 1 }, mosaic, cup))
                    current.offset.Y++;  // жорстке падіння
                break;
            case Key::PAUSE:  // ЛОГІКА ПАУЗИ
            {
                // Збереження досягнень на всяк випадок:
                if (stats.score >= stats.highScore) {  // не '>' !!!
                    stats.highScore = stats.score;
                    util_SaveHighScore(stats);
                }
                // Установлення позиції повідомлення про паузу
                short pauseX = cup.startX - 20;  // має бути константа...
                short pauseY = cup.startY + cup.height * 1 / 3;
                // Відображення повідомлення про паузу та її скасування
                util_ShowText(h, pauseX, pauseY, "         ПАУЗА",
                    Color::YELLOW);
                util_ShowText(h, pauseX, pauseY + 2, "Натисніть ESC ще раз",
                    Color::YELLOW);
                // Очікування на повторне натискання клавіші
                while (true) {
                    if (_kbhit()) {
                        int input = _getch();
                        if (input == static_cast<int>(Key::PAUSE))
                            break;  // завершення паузи
                    }
                    Sleep(TetrisConstants::SLEEP_DELAY);
                }
                // Видалення повідомлення з консолі
                util_ShowText(h, pauseX, pauseY, string(20, ' '),
                    Color::BLACK);
                util_ShowText(h, pauseX, pauseY + 2, string(20, ' '),
                    Color::BLACK);
            }
            break;
            case Key::RESTART:  // ЛОГІКА ПЕРЕЗАПУСКУ (F1 - така поміч)
                // Очищення ігрової мозаїки
                for (short y = 0; y < cup.height; y++) {
                    for (short x = 0; x < cup.width; x++) {
                        mosaic[y][x] = Color::BLACK;
                    }
                }
                // Очищення області бонусного повідомлення
                util_ShowText(h, bonusPos.X, bonusPos.Y, string(9, ' '),
                    Color::BLACK);
                // Збереження досягнень на всяк випадок:
                if (stats.score >= stats.highScore) {  // не '>' !!!
                    stats.highScore = stats.score;
                    util_SaveHighScore(stats);
                }
                // Скидання статистики
                stats.score = 0;
                stats.level = 1;  // не реалізовано вибір рівня...
                // Створення нових тетраміно
                current = v4_CreateTetromino(static_cast<Type>(rand() % 7), cup);
                next = v4_CreateTetromino(static_cast<Type>(rand() % 7), cup);
                // Повернення швидкості гри до початкового рівня
                tick = TetrisConstants::INITIAL_TICK;
                // Скидання стану гри
                gameOver = false;
                restart = true;
                break;
            }
        }

        if (restart) continue;  // повторне проходження ігрового циклу

        // Перевірка часу для автоматичного падіння
        if (clock() - last >= tick) {  // з моменту останнього оновлення
            // Очищення старого положення тетраміно
            v4_EraseTetromino(h, current);
            // Перевірка можливості руху вниз на одну клітинку
            if (v4_CanMove(current, { 0, 1 }, mosaic, cup)) {
                current.offset.Y++;
            }
            else {  // коли рух вниз неможливий
                // Закріплення тетраміно в ігровій мозаїці
                v4_FixTetromino(current, mosaic, cup);
                // Очищення заповнених ліній (максимум 4 лінії!)
                v4_CheckLines(h, mosaic, cup, stats, bonusPos);
                // Оновлення швидкості гри залежно від рівня
                tick = max(TetrisConstants::INITIAL_TICK -
                    (stats.level - 1) * TetrisConstants::TICK_STEP,
                    TetrisConstants::MIN_TICK);
                current = next;  // наступне тетраміно стає поточним
                next = v4_CreateTetromino(static_cast<Type>(rand() % 7), cup);
                if (v4_CheckGameOver(mosaic, cup, current))
                    gameOver = true;  // гра завершується,
                // якщо нова фігура не може з'явитися через заповнене поле 
            }
            last = clock();  // оновлення часу останнього падіння
        }

        // Малювання всіх елементів інтерфейсу
        v4_DrawGlass(h, cup);
        v4_DrawMosaic(h, mosaic, cup);
        v4_DrawCurrentTetromino(h, current);
        v4_DrawNextTetromino(h, next, nextPos);
        v4_DrawStatistics(h, stats, statsPos);

        Sleep(TetrisConstants::SLEEP_DELAY);
    } while (!gameOver);

    // Оновлення рекорду, збереження у файл
    if (stats.score >= stats.highScore) {  // не '>' !!!
        // Оператор порівняння '>=' гарантує створення файлу в першій грі!
        stats.highScore = stats.score;
        util_SaveHighScore(stats);
    }

    // Обробка завершення гри
    bool restart = false;
    short msgX = cup.startX - TetrisConstants::MARGIN_LEFT;
    short msgY = cup.startY * 2 / 3;
    v4_DrawGameOver(h, cup, stats, restart);
    if (restart) {
        // Очищення області повідомлення про завершення гри
        util_ShowText(h, msgX, msgY, string(25, ' '), Color::BLACK);
        util_ShowText(h, msgX, msgY + 2, string(25, ' '), Color::BLACK);
        util_ShowText(h, msgX, msgY + 3, string(25, ' '), Color::BLACK);
        util_ShowText(h, msgX, msgY + 5, string(25, ' '), Color::BLACK);

        // Очищення консолі
        util_RestoreConsoleState(h);

        // Повторна ініціалізація налаштувань консолі
        util_SetConsoleOptions(h);

        // Логіка рестарту (пор. case Key::RESTART)
        for (short y = 0; y < cup.height; y++) {
            for (short x = 0; x < cup.width; x++) {
                mosaic[y][x] = Color::BLACK;
            }
        }
        stats.score = 0;
        stats.level = 1;
        current = v4_CreateTetromino(static_cast<Type>(rand() % 7), cup);
        next = v4_CreateTetromino(static_cast<Type>(rand() % 7), cup);
        tick = TetrisConstants::INITIAL_TICK;
        gameOver = false;

        // Перемалювання всіх елементів інтерфейсу
        v4_DrawGlass(h, cup);
        v4_DrawMosaic(h, mosaic, cup);
        v4_DrawControls(h, { static_cast<SHORT>(cup.startX + cup.width +
            TetrisConstants::MARGIN_RIGHT), TetrisConstants::CONTROLS_Y });
        v4_DrawNextTetromino(h, next, nextPos);
        v4_DrawStatistics(h, stats, statsPos);

        // Очищення області бонусного привітання
        util_ShowText(h, bonusPos.X, bonusPos.Y, string(9, ' '), Color::BLACK);
        // https://en.cppreference.com/w/cpp/language/goto
        goto restart;  // повернення до основного циклу!
    }

    // Вихід із гри
    v4_DeleteMosaic(mosaic, cup.height);
    util_RestoreConsoleState(h);
    return 0;
}