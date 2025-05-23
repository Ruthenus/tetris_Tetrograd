#include "tetrograd_functions.h"
#include <iostream>
#include <cstdlib>  // srand(), rand()
#include <ctime>    // time()

using namespace std;


/**
 * Допоміжна функція для виведення тексту в заданій позиції на консолі.
 * Передає Color за значенням без константного посилання, оскільки
 * базовий тип unsigned char (8 біт) достатній для всіх значень (0–15).
 * Параметри функції:
 * - h: дескриптор консолі для виведення тексту в консоль;
 * - x: значення типу short (X-координата позиції);
 * - y: значення типу short (Y-координата позиції);
 * - text: константне посилання на об'єкт std::string для
 * виведення тексту;
 * - color: значення типу Color (перелік для кольору тексту).
 * Return: немає (void). Функція виводить текст у консоль.
 * Використовує константне посилання (const string&) для ефективної
 * передачі рядка без копіювання.
 */
void util_ShowText(HANDLE h, short x, short y, const string& text,
    Color color)
{
    SetConsoleOutputCP(1251);  // для коректного відображення кирилиці
    // Приведення Color до WORD виконується тут, щоб спростити виклики 
    // функції.
    SetConsoleTextAttribute(h, static_cast<WORD>(color));
    // SetConsoleCursorPosition використовує COORD {x, y} для 
    // позиціонування курсора:
    SetConsoleCursorPosition(h, { x, y });
    cout << text;  // виводимо заданий текст у заданій позиції {x, y}
}


/**
 * Функція ініціалізації налаштувань консолі.
 * Допоміжна функція, яка забезпечує коректну роботу консольної
 * програми, зокрема різновиду гри "Тетріс" – "Тетроград". Функція
 * відповідає за встановлення кодової сторінки для підтримки кирилиці,
 * ініціалізацію генератора випадкових чисел, встановлення заголовка
 * вікна консолі, приховування курсору для чистоти інтерфейсу та
 * реєстрацію обробника системних сигналів.
 * Параметри функції:
 * - h - покажчик типу HANDLE (дескриптор консолі), отриманий через
 * GetStdHandle(STD_OUTPUT_HANDLE).
 * Return: немає (void). Функція змінює стан консолі через покажчик h
 * та не повертає значення.
 * Функція викликається на початку програми для підготовки консолі.
 */
void util_SetConsoleOptions(HANDLE h)
{
    // Установлення кодової сторінки 1251 для підтримки кирилиці
    SetConsoleOutputCP(1251);

    // Ініціалізація генератора випадкових чисел на основі поточного часу
    srand(static_cast<unsigned>(time(nullptr)));
    // nullptr – початок епохи UNIX 1 січня 1970 року, 00:00:00 UTC

    // Установлення заголовка вікна консолі
    SetConsoleTitleA("TETROGRAD");
    // якщо запускаєте .exe, а не з VS, працюватиме як очікується
    // https://learn.microsoft.com/en-us/windows/console/setconsoletitle

    // Налаштування параметрів курсору
    CONSOLE_CURSOR_INFO cursor = { 100, FALSE };
    // cursor.dwSize = 100;  // розмір 100 (повне заповнення клітинки)
    // cursor.bVisible = false;  // видимість false (прихований)
    // https://learn.microsoft.com/en-us/windows/console/console-cursor-info-str
    // Застосування налаштувань для приховування курсору
    SetConsoleCursorInfo(h, &cursor);
    // https://learn.microsoft.com/en-us/windows/console/setconsolecursorinfo

    //// Установлення обробника сигналів консолі
    //SetConsoleCtrlHandler(util_ConsoleHandler, TRUE);
    //// https://learn.microsoft.com/en-us/windows/console/setconsolectrlhandler
}


/**
* Обробник сигналів консолі - допоміжна функція гри "Тетроград", яка
* забезпечує коректне завершення програми в разі виникненні подій,
* таких як Ctrl+C, штатне закриття консолі або вимкнення системи.
* Дублює логіку збереження прогресу з main(): перевіряє, чи поточний
* рахунок (stats.score) перевищує найвищий рахунок (stats.highScore),
* і якщо так, оновлює highScore та зберігає рекорд у файл за допомогою
* util_SaveHighScore. Для системних подій (CTRL_LOGOFF_EVENT,
* CTRL_SHUTDOWN_EVENT) зберігає прогрес без перевірки, щоб гарантувати
* збереження. Відновлює стан консолі перед завершенням та повертає
* TRUE, якщо сигнал оброблено, або FALSE для невідомих сигналів.
* Параметри функції:
* - signal: код події (DWORD), що вказує на тип сигналу.
* Return: BOOL (TRUE або FALSE).
*/
BOOL WINAPI util_ConsoleHandler(DWORD signal)
// https://learn.microsoft.com/en-en/windows/console/handlerroutine
{
    // Статичний покажчик на GameStats, оголошений усередині функції
    static GameStats* activeStats = nullptr;

    // Отримуємо дескриптор стандартного виводу консолі для відновлення 
    // стану консолі перед завершенням програми
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hConsole == INVALID_HANDLE_VALUE) {
        return FALSE;  // якщо консоль недоступна
    }

    // Перевірка на nullptr перед використанням
    if (!activeStats) {
        util_RestoreConsoleState(hConsole);
        return FALSE;
    }

    // Аналізуємо тип сигналу для вибору відповідної логіки обробки:
    switch (signal) {
        // Обробка сигналів, викликаних користувачем у консолі
    case CTRL_C_EVENT:  // сигнал Ctrl+C переривання поточної задачі
    case CTRL_BREAK_EVENT:  // сигнал Ctrl+Break, якщо є клавіша Break
        // Дублюємо логіку збереження рекорду з main():
        if (activeStats->score > activeStats->highScore) {
            activeStats->highScore = activeStats->score;
            util_SaveHighScore(*activeStats);
        }
        SetConsoleOutputCP(1251);
        // Відновлюємо початковий стан консолі
        util_RestoreConsoleState(hConsole);
        // Завершуємо процес із кодом 0 (успішне завершення)
        ExitProcess(0);
        // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess
        break;
        // Обробка сигналу закриття вікна консолі натисканням кнопки "X"
    case CTRL_CLOSE_EVENT:  // штатне закриття консолі користувачем
        if (activeStats->score > activeStats->highScore) {
            activeStats->highScore = activeStats->score;
            util_SaveHighScore(*activeStats);
        }
        SetConsoleOutputCP(1251);
        util_RestoreConsoleState(hConsole);
        // Додаємо мінімальну затримку (0.5 секунди) для надійного 
        // завершення операцій збереження у файл. Відомо: Windows 
        // обмежує обробку CTRL_CLOSE_EVENT до ~5 секунд.
        Sleep(500);
        ExitProcess(0);  // відтак завершуємо процес
        break;
        // Обробка системних подій: вихід, вимкнення, перезавантаження
    case CTRL_LOGOFF_EVENT:  // користувач виходить із системи
    case CTRL_SHUTDOWN_EVENT: // вимкнення або перезавантаження системи
        // Для системних подій зберігаємо прогрес без перевірки,
        // щоб гарантувати збереження даних, навіть якщо рахунок 
        // не перевищує найвищий. На жаль, досягнений рекорд пропаде.
        util_SaveHighScore(*activeStats);
        SetConsoleOutputCP(1251);
        util_RestoreConsoleState(hConsole);
        ExitProcess(0);
        break;
        // Обробка невідомих сигналів
    default:
        // Повертаємо FALSE, щоб система могла передати сигнал іншим 
        // обробникам або виконати дію за замовчуванням
        return FALSE;
    }
    // Повертаємо TRUE, щоб повідомити систему, що сигнал успішно 
    // оброблено, запобігаючи подальшій обробці іншими обробниками
    return TRUE;
}


/**
 * Допоміжна функція ініціалізації статистики для обробника.
 * Ініціалізує покажчик на структуру GameStats, присвоюючи йому адресу
 * переданої структури stats. Запобігає витокам пам'яті.
 * Параметри функції:
 * - stats: покажчик на структуру GameStats, що містить статистику гри;
 * - activeStats: подвійний покажчик на структуру GameStats, що
 * буде оновлено, зберігаючи адресу stats для подальшого використання.
 */
void util_InitHandlerStats(GameStats* stats, GameStats** activeStats)
{
    // Запобігання використанню нульових покажчиків
    if (!stats || !activeStats) return;
    // Пряме присвоєння через покажчик на покажчик
    *activeStats = stats;
    // Тепер activeStats вказує на ту ж область пам’яті, що і stats.
    // Операція дозволяє змінити значення покажчика поза межами функції, 
    // що корисно для керування станом гри.
}


/**
 * Допоміжна функція відновлення стану консолі.
 * Функція скидає налаштування консолі, встановлені раніше, наприклад,
 * у util_SetConsoleOptions. Вона очищає екран, відновлює стандартний
 * колір тексту, параметри курсора, кодову сторінку й заголовок консолі.
 * Використовується для повернення консолі до початкового стану після
 * виконання програми або певного режиму.
 * Параметри функції:
 * - h: дескриптор консолі, отриманий, наприклад, за допомогою
 * GetStdHandle(STD_OUTPUT_HANDLE).
 * Return: немає (void).
 */
void util_RestoreConsoleState(HANDLE h)
{
    // Відновлення стандартного кольору тексту (яскраво-білий)
    SetConsoleTextAttribute(h, static_cast<WORD>(Color::BRIGHT_WHITE));

    // Відновлення параметрів курсора -
    CONSOLE_CURSOR_INFO cursor = { 25, TRUE };
    // стандартний розмір 25, видимий курсор
    SetConsoleCursorInfo(h, &cursor);

    // Отримання інформації про буфер екрана консолі
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    // https://learn.microsoft.com/en-us/windows/console/console-screen-buffer-info-str
    GetConsoleScreenBufferInfo(h, &csbi);
    // https://learn.microsoft.com/en-us/windows/console/getconsolescreenbufferinfo
    // Обчислення розміру буфера (ширина * висота)
    DWORD size = csbi.dwSize.X * csbi.dwSize.Y;

    // Очищення екрана: заповнення пробілами
    DWORD written;  // lpNumberOfCharsWritten, lpNumberOfAttrsWritten
    FillConsoleOutputCharacter(h, ' ', size, { 0, 0 }, &written);
    // https://learn.microsoft.com/en-us/windows/console/fillconsoleoutputcharacter
    // Відновлення атрибутів (кольорів) для всіх символів у буфері
    FillConsoleOutputAttribute(h, csbi.wAttributes, size, { 0, 0 }, &written);
    // https://learn.microsoft.com/en-us/windows/console/fillconsoleoutputattribute

    // Переміщення курсора в початкову позицію
    SetConsoleCursorPosition(h, { 0, 0 });

    // Відновлення кодової сторінки до стандартної системної
    SetConsoleOutputCP(GetOEMCP());
    SetConsoleCP(GetOEMCP());
    // https://learn.microsoft.com/en-us/windows/win32/api/winnls/nf-winnls-getoemcp

    // Відновлення стандартного заголовка консолі
    SetConsoleTitleA("");
}


/**
 * Допоміжна функція, що отримує розміри вікна консолі.
 * Параметри функції:
 * - h: дескриптор консолі;
 * - outWidth: посилання на змінну типу short, в яку буде записана
 * ширина консолі в символах;
 * - outHeight: посилання на змінну типу short, в яку буде записана
 * висота консолі в символах.
 * Return: немає (void).
 * Записує результат в параметри outWidth та outHeight.
 */
void util_GetConsoleSize(HANDLE h, short& outWidth, short& outHeight)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    // Отримуємо інформацію про буфер екрана консолі
    GetConsoleScreenBufferInfo(h, &csbi);
    // Обчислюємо ширину за правим і лівим краями вікна
    outWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    // Обчислюємо висоту за нижнім і верхнім краями вікна
    outHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}


// Завантаження рекордного рахунку
int util_LoadHighScore()  // тема наступного домашнього завдання
{
    // Функція залишена для сумісності, але не виконує жодних дій
    return 0;
}


// Збереження рекордного рахунку
void util_SaveHighScore(const GameStats& stats)  // тема наступного ДЗ
{
    // Функція залишена для сумісності, але не виконує жодних дій
}