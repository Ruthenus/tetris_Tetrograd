#include "tetrograd_functions.h"
#include <iostream>
#include <conio.h>  // _getch(), _kbhit() — для роботи з клавіатурою

using namespace std;


/**
 * Функція створення нової фігури тетраміно, ініціалізації її
 * властивостей та позиціювання в ігровій структурі.
 * Параметри функції:
 * – type: значення типу Type (перелік форм тетраміно);
 * - cup: константне посилання на структуру Glass для центрування
 * тетраміно.
 * Return: Tetromino. Повертає структуру Tetromino, ініціалізовану з
 * типом, кольором, координатами квадратів і зсувом.
 * У структурі Tetromino є масив квадратів squares[4] типу COORD
 * (статичний масив фіксованого розміру).
 */
Tetromino v4_CreateTetromino(Type type, const Glass& cup)
{   // Промодельовано взаємодію з функцією v4_RotateTetromino()

    // Створення фігури структурного типу тетраміно:
    Tetromino tm;
    tm.type = type;

    // Ініціалізація кольору та локальних координат квадратів
    switch (type) {  // залежно від типу тетраміно
    case Type::I:
        tm.color = Color::AQUA;   // ретро-ефекти вимкнено ? LIGHT_AQUA
        tm.squares[0] = { 0, 0 };
        tm.squares[1] = { 1, 0 };  // центр обертання
        tm.squares[2] = { 2, 0 };
        tm.squares[3] = { 3, 0 };
        break;
        /*
          | 0 1 2 3 4
         -------------
         0| 0 1 2 3
         1|
         2|
         3|
         4|
        */
    case Type::O:
        tm.color = Color::YELLOW; // ретро-ефекти вимкн. ? LIGHT_YELLOW
        tm.squares[0] = { 0, 1 };
        tm.squares[1] = { 1, 1 };
        tm.squares[2] = { 0, 2 };
        tm.squares[3] = { 1, 2 };
        break;
        /*
          | 0 1 2 3 4
         -------------
         0|
         1| 0 1
         2| 2 3
         3|
         4|
        */
    case Type::T:
        tm.color = Color::PURPLE;  // ретро-ефекти вимкн.? LIGHT_PURPLE
        tm.squares[0] = { 0, 1 };
        tm.squares[1] = { 1, 1 };  // центр обертання
        tm.squares[2] = { 2, 1 };
        tm.squares[3] = { 1, 2 };
        break;
        /*
          | 0 1 2 3 4
         -------------
         0|
         1| 0 1 2
         2|   3
         3|
         4|
        */
    case Type::J:
        tm.color = Color::BLUE;
        tm.squares[0] = { 0, 1 };
        tm.squares[1] = { 0, 2 };  // центр обертання
        tm.squares[2] = { 1, 2 };
        tm.squares[3] = { 2, 2 };
        break;
        /*
          | 0 1 2 3 4
         -------------
         0|
         1| 0
         2| 1 2 3
         3|
         4|
        */
    case Type::L:
        tm.color = Color::LIGHT_RED;  // у класичній грі помаранчевий!
        tm.squares[0] = { 2, 1 };
        tm.squares[1] = { 2, 2 };  // центр обертання
        tm.squares[2] = { 1, 2 };
        tm.squares[3] = { 0, 2 };
        break;
        /*
          | 0 1 2 3 4
         -------------
         0|
         1|     0
         2| 3 2 1
         3|
         4|
        */
    case Type::S:
        tm.color = Color::GREEN;  // ретро-ефекти вимкнено? LIGHT_GREEN
        tm.squares[0] = { 1, 2 };
        tm.squares[1] = { 0, 2 };  // центр обертання
        tm.squares[2] = { 0, 3 };
        tm.squares[3] = { 1, 1 };
        break;
        /*
          | 0 1 2 3 4
         -------------
         0|
         1|   3
         2| 1 0
         3| 2
         4|
        */
    case Type::Z:
        tm.color = Color::RED;     // ретро-ефекти вимкнено ? LIGHT_RED
        tm.squares[0] = { 0, 1 };
        tm.squares[1] = { 1, 1 };  // центр обертання
        tm.squares[2] = { 1, 2 };
        tm.squares[3] = { 2, 2 };
        break;
        /*
          | 0 1 2 3 4
         -------------
         0|
         1| 0 1
         2|   2 3
         3|
         4|
        */
    }

    // Обчислення ширини тетраміно по осі X
    short minX = tm.squares[0].X;  // найлівіша X-координата
    short maxX = tm.squares[0].X;  // найправіша X-координата
    for (short i = 1; i < 4; i++) {
        minX = min(minX, tm.squares[i].X);
        maxX = max(maxX, tm.squares[i].X);
    }
    short tmWidth = maxX - minX + 1;  // ширина тетраміно

    // Центруємо тетраміно по горизонталі в ігровій структурі
    tm.offset.X = cup.startX + (cup.width - tmWidth) / 2;
    // Задаємо початкову позицію по вертикалі –
    tm.offset.Y = 0; // початок падіння з верхнього краю консолі!

    // Повертаємо ініціалізоване тетраміно
    return tm;
}


/**
 * Функція створення ігрової мозаїки для гри "Тетроград" – двовимірного
 * масиву, який представляє стан т.зв. ігрового поля, де кожна клітинка
 * зберігає колір блоку (чорний для порожньої клітинки).
 * Параметри функції:
 * - cup: константне посилання на структуру Glass (height, width,
 * startX, startY) для визначення розмірів мозаїки.
 * Return: Color**. Повертає покажчик на масив покажчиків (Color**),
 * де кожен елемент — це динамічно виділений масив типу Color
 * (рядок мозаїки).
 * Динамічна пам'ять виділяється оператором new і потребує
 * звільнення через v4_DeleteMosaic.
 */
Color** v4_CreateMosaic(const Glass& cup)
{
    // Створюємо масив покажчиків для рядків ігрової структури
    Color** mosaic = new Color * [cup.height];
    // Ітеруємо по рядках двовимірного масиву mosaic:
    for (short i = 0; i < cup.height; i++) {
        // Виділяємо пам'ять для стовпців у поточному рядку
        mosaic[i] = new Color[cup.width];

        // Перевіряємо, чи вдало виділена пам'ять
        if (!mosaic[i]) {
            // Очищаємо пам’ять для всіх попередніх рядків перед виходом
            for (short j = 0; j < i; j++) delete[] mosaic[j];
            // Очищаємо масив покажчиків, щоб уникнути витоків пам'яті
            delete[] mosaic;
            return nullptr;
        }

        // Ініціалізуємо всі клітинки рядка як порожні:
        for (short j = 0; j < cup.width; j++)
            mosaic[i][j] = Color::BLACK;
    }

    // Повертаємо покажчик на створений двовимірний масив
    return mosaic;  // покажчик на покажчик кольорів
}


/**
 * Функція звільнення пам'яті від ігрової мозаїки.
 * Параметри функції:
 * - mosaic: покажчик на двовимірний масив типу Color**
 * (ігрова мозаїка).
 * - height: значення типу short (висота мозаїки, збігається з
 * cup.height) для ітерації по рядках.
 * Return: немає (void). Функція звільняє пам'ять, виділену для мозаїки.
 * Функція використовує delete[] для звільнення кожного рядка,
 * а потім delete[] для масиву покажчиків.
 */
void v4_DeleteMosaic(Color** mosaic, short height)
{
    // Ітеруємо по всіх рядках двовимірного масиву mosaic:
    for (short i = 0; i < height; i++)  // height = cup.height
        // Звільняємо пам’ять, виділену для стовпців у поточному рядку
        delete[] mosaic[i];
    // Звільняємо пам’ять, виділену для масиву покажчиків на рядки
    delete[] mosaic;
}


/**
 * Функція обертання тетраміно на 90 градусів за годинниковою стрілкою
 * навколо центру обертання. Використовує механізм "wall kicks" для
 * уникнення колізій з боковими гранями, дном або іншими тетраміно.
 * Якщо обертання неможливе через колізії, тетраміно залишається в
 * початковому стані.
 * Параметри функції:
 * - tm: посилання на структуру Tetromino, що модифікується
 * під час обертання;
 * - mosaic: двовимірний масив Color** для перевірки колізій;
 * - cup: структура Glass для перевірки меж стакана.
 * Return: немає (void). Функція змінює координати та зсув tm в разі
 * успішного обертання або залишає без змін, якщо обертання неможливе.
 * Використовує посилання (Tetromino&) для зміни структури tm.
 * Використовує локальний статичний масив kicks[7][2] для зсувів
 * ("wall kicks") і тимчасовий масив newSquares[4] для нових координат.
 */
void v4_RotateTetromino(Tetromino& tm, Color** mosaic, const Glass& cup)
{  // Промодельовано взаємодію з функцією v4_CreateTetromino()

    // Тетраміно O не обертається через свою симетрію (квадрат 2x2):
    if (tm.type == Type::O) return;

    // Визначаємо центр обертання (другий квадрат у масиві tm.squares):
    COORD center = tm.squares[1];  // для всіх тетраміно (крім O)
    // Для перевірки колізій створюємо масив для тимчасового зберігання
    // нових координат квадратів тетраміно після обертання
    COORD newSquares[4];

    // Обчислюємо нові координати квадратів після обертання 
        // https://uk.wikipedia.org/wiki/Матриця_повороту
        // [ 0  -1
        //   1   0 ]
    // на 90 градусів за годинниковою стрілкою:
    for (short i = 0; i < 4; i++) {
        short dx = tm.squares[i].X - center.X; // відносний зсув по X
        short dy = tm.squares[i].Y - center.Y; // відносний зсув по Y
        newSquares[i].X = center.X + dy;       // нова координата X
        newSquares[i].Y = center.Y - dx;       // нова координата Y
    }

    // Застосовуємо масив зсувів ("wall kicks") для уникнення колізій.
    // Кожна пара {dx, dy} визначає спробу зсуву тетраміно після обертання:
    short kicks[7][2] = {  // "wall kicks" оптимізовано для тетраміно I
        { 0, 0 },   // без зсуву
        { -1, 0 },  // зсув на 1 клітинку ліворуч
        { 1, 0 },   // зсув на 1 клітинку праворуч
        { -2, 0 },  // зсуви на 2 клітинку ліворуч
        { 2, 0 },   // зсуви на 2 клітинку праворуч
        { 0, 1 },   // зсув на 1 клітинку вгору
        { 0, -1 }   // зсув на 1 клітинку вниз
    };

    // Перебираємо всі можливі зсуви з масиву kicks:
    for (auto& k : kicks) {
        // Ставимо прапорець для перевірки,
        bool ok = true;  // чи можливе обертання з поточним зсувом

        // Перевіряємо кожен квадрат тетраміно після обертання:
        for (short j = 0; j < 4; j++) {
            // Абсолютні координати нового квадрата в ігровій структурі
            short x = newSquares[j].X + tm.offset.X + k[0];
            short y = newSquares[j].Y + tm.offset.Y + k[1];

            // Перевіряємо, чи виходить квадрат за межі структури:
            if (x < cup.startX || x >= cup.startX + cup.width ||
                y >= cup.startY + cup.height) {
                ok = false;  // якщо поза межею, обертання неможливе
                break;
            }

            // Перевіряємо, чи не перетинається квадрат із зайнятими 
            // клітинками в ігровій мозаїці. Якщо квадрат знаходиться в
            // межах видимої структури і клітинка не порожня, є колізія.
            if (y >= cup.startY &&
                mosaic[y - cup.startY][x - cup.startX] != Color::BLACK) {
                ok = false;  // А якщо є колізія, обертання неможливе
                break;
            }
        }

        // Якщо обертання можливе з поточним зсувом (немає колізій),
        // застосовуємо нові координати квадратів та оновлюємо зсуви:
        if (ok) {
            for (short j = 0; j < 4; j++) {
                // Оновлюємо локальні координати квадратів
                tm.squares[j] = newSquares[j];
            }
            tm.offset.X += k[0];  // оновлюємо зсув по X
            tm.offset.Y += k[1];  // оновлюємо зсув по Y

            return;  // завершуємо функцію після успішного обертання
        }
    }

    // Якщо жоден зсув не дозволив обертання (усі спроби призвели до 
    // колізій), функція завершується без змін у тетраміно 
    // (tm залишається в початковому стані).
}


/**
 * Функція перевірки, чи може тетраміно переміститися на задану відстань
 * {delta.X, delta.Y} без виходу за межі ігрової структури та без
 * колізій з іншими тетраміно.
 * Параметри функції:
 * - tm: константне посилання на структуру Tetromino (тип, координати
 * квадратів, колір, зсув);
 * - delta: структура COORD із зсувами по X та Y (delta.X, delta.Y)
 * для руху;
 * - mosaic: покажчик на двовимірний масив типу Color**
 * (ігрова мозаїка);
 * - cup: константне посилання на структуру Glass для перевірки меж.
 * Return: bool. Повертає true, якщо переміщення можливе без колізій,
 * і false, якщо є колізії або вихід за межі стакана.
 * У структурі Tetromino є статичний масив squares[4].
 */
bool v4_CanMove(const Tetromino& tm, COORD delta, Color** mosaic,
    const Glass& cup)
{  // Пор. коментарі до функції v4_RotateTetromino()
    for (short i = 0; i < 4; i++) {
        // Нові абсолютні координати квадрата після переміщення:
        short x = tm.squares[i].X + tm.offset.X + delta.X;
        short y = tm.squares[i].Y + tm.offset.Y + delta.Y;

        if (x < cup.startX || x >= cup.startX + cup.width ||
            y >= cup.startY + cup.height)  // Виходить за межі?
            return false;  // Тоді рух неможливий

        // До речі, mosaic – видима частина ігрової структури cup
        if (y >= cup.startY && mosaic[y - cup.startY][x - cup.startX]
            != Color::BLACK)  // Є колізія?
            return false;  // Отже, рух неможливий
    }

    // Якщо жоден квадрат не викликав колізію або не вийшов за межі,
    return true;  // рух можливий!
}


/**
 * Функція закріплення тетраміно в ігровій мозаїці, яка додає квадрати
 * тетраміно до мозаїки. Після фіксації тетраміно стає частиною мозаїки
 * та більше не може рухатися. Функція не повертає значення, але
 * змінює масив ігрової мозаїки!
 * Параметри функції:
 * - tm: константне посилання на структуру Tetromino, квадрати якої
 * фіксуються;
 * - mosaic: покажчик на двовимірний масив типу Color** для додавання
 * квадратів тетраміно;
 * - cup: константне посилання на структуру Glass для координат у
 * мозаїці.
 * Return: немає (void). Модифікує mosaic, додаючи квадрати тетраміно.
 * У структурі Tetromino є статичний масив squares[4].
 */
void v4_FixTetromino(const Tetromino& tm, Color** mosaic, const Glass& cup)
{
    for (short i = 0; i < 4; i++) {
        // Координати квадрата в системі координат масиву mosaic:
        short x = tm.squares[i].X + tm.offset.X - cup.startX;
        short y = tm.squares[i].Y + tm.offset.Y - cup.startY;

        if (y >= 0 && y < cup.height && x >= 0 && x < cup.width)
            // фіксація квадрата в мозаїці кольором тетраміно
            mosaic[y][x] = tm.color;
    }
}


/**
 * Функція для стирання тетраміно з консолі й підготовки до
 * перемалювання.
 * Параметри функції:
 * - h: дескриптор консолі для стирання тетраміно;
 * - tm: константне посилання на структуру Tetromino, чиї квадрати
 * замінюються пробілами.
 * Return: немає (void). Стирає тетраміно з консолі.
 * У структурі Tetromino є статичний масив squares[4].
 */
void v4_EraseTetromino(HANDLE h, const Tetromino& tm)
{
    // Встановлення кодової сторінки 437 (OEM US) для коректного 
    // відображення псевдографічних символів
    SetConsoleOutputCP(437);  // додано під час тестування гри

    // Встановлення чорного кольору тексту для "стирання"
    SetConsoleTextAttribute(h, static_cast<WORD>(Color::BLACK));
    // (збігається з фоном консолі)

    for (short i = 0; i < 4; i++) {
        COORD pos;  // абсолютні координати квадрата тетраміно
        pos.X = tm.squares[i].X + tm.offset.X;
        pos.Y = tm.squares[i].Y + tm.offset.Y;

        // Переміщення курсору в обчислену позицію
        SetConsoleCursorPosition(h, pos);
        // Виведення пробілу для заміни квадрата тетраміно
        cout << ' ';  // (ефект стирання)
    }
}


/**
 * Функція для відображення тетраміно на консолі у заданій позиції,
 * ігноруючи його поточне зміщення. Функція не повертає значення, але
 * виводить символи тетраміно на консоль.
 * Параметри функції:
 * - h: дескриптор консолі для виведення тетраміно;
 * - tm: константне посилання на структуру Tetromino (форма, колір,
 * координати);
 * - position: структура COORD для базової позиції малювання;
 * Return: немає (void). Малює тетраміно в заданій позиції.
 * У структурі Tetromino є статичний масив squares[4].
 */
void v4_ShowTetrominoAt(HANDLE h, const Tetromino& tm, COORD position)
{
    SetConsoleOutputCP(437);  // додано під час тестування гри
    // Встановлення кольору тексту для виведення тетраміно
    SetConsoleTextAttribute(h, static_cast<WORD>(tm.color));

    for (short i = 0; i < 4; i++) {
        // Обчислення позиції символу тетраміно на консолі:
        COORD coutPosChar;
        coutPosChar.X = position.X + tm.squares[i].X;
        coutPosChar.Y = position.Y + tm.squares[i].Y;

        SetConsoleCursorPosition(h, coutPosChar);
        cout << TetrisConstants::TETROMINO_CHAR;  // символ тетраміно
    }
}


/**
 * Функція для малювання тетраміно на консолі в його поточній позиції,
 * використовуючи зміщення. Функція викликає v4_ShowTetrominoAt(), що є
 * частиною механізму малювання, із позицією, яка відповідає tm.offset.
 * Параметри функції:
 * - h: дескриптор консолі для виведення поточного тетраміно;
 * - tm: константне посилання на структуру Tetromino із координатами
 * та зсувом.
 * Return: немає (void). Малює поточне тетраміно в його позиції.
 */
void v4_DrawCurrentTetromino(HANDLE h, const Tetromino& tm)
{
    COORD position = { tm.offset.X, tm.offset.Y };
    v4_ShowTetrominoAt(h, tm, position);
}


/**
 * Функція для малювання наступного тетраміно в заданій позиції на
 * консолі. Очищає область, де буде намальовано тетраміно, і викликає
 * функцію v4_ShowTetrominoAt() для малювання наступного тетраміно.
 * Параметри функції:
 * - h: дескриптор консолі для виведення наступного тетраміно;
 * - next: константне посилання на структуру Tetromino
 * (наступне тетраміно);
 * - nextPos: структура COORD для позиції малювання.
 * Return: Немає (void). Очищає область і малює наступне тетраміно.
 * У next є статичний масив squares[4].
 */
void v4_DrawNextTetromino(HANDLE h, const Tetromino& next, COORD nextPos)
{
    // Очищення області 5x5, де буде відображатися наступне тетраміно.
    // Це необхідно, щоб видалити попереднє тетраміно або залишки символів.
    for (short y = 0; y < 5; y++) {
        for (short x = 0; x < 5; x++) {
            // Виведення пробілу в заданій позиції з кольором BLACK, 
            // щоб забезпечити чистий фон
            util_ShowText(h, nextPos.X + x, nextPos.Y + y, " ", Color::BLACK);
        }
    }

    // Малювання наступного тетраміно у зазначеній позиції на консолі
    v4_ShowTetrominoAt(h, next, nextPos);
}


/**
 * Функція перевірки та очищення заповнених ліній, оновлення статистики
 * (рахунок, рівень, рекорд), а також відображення бонусного привітання
 * з ефектом миготіння на заданій позиції у разі очищення 4 ліній.
 * Параметри функції:
 * - h: дескриптор консолі для виведення бонусного повідомлення;
 * - mosaic: покажчик на двовимірний масив типу Color** для
 * очищення ліній;
 * - cup: константне посилання на структуру Glass для розмірів мозаїки;
 * - stats: посилання на структуру GameStats для оновлення статистики;
 * - bonusPos: структура COORD для позиції повідомлення "TETRIS!".
 * Return: немає (void). Модифікує mosaic (очищає лінії) і stats.
 * Використовує динамічно виділений масив fullLines типу bool* для
 * позначення заповнених ліній (звільняється через delete[]).
 */
void v4_CheckLines(HANDLE h, Color** mosaic, const Glass& cup,
    GameStats& stats, COORD bonusPos)
{
    short cleared = 0;  // лічильник очищених ліній
    // Масив для позначення заповнених ліній (ініціалізовано false):
    bool* fullLines = new bool[cup.height]();  // +1 динамічний масив

    // Крок 1: Виявлення всіх заповнених ліній
    for (short y = 0; y < cup.height; y++) {
        bool full = true;  // припустимо, що лінія заповнена
        for (short x = 0; x < cup.width; x++) {
            if (mosaic[y][x] == Color::BLACK) {
                full = false;
                break;  // якщо є чорна клітинка, то лінія не заповнена
            }
        }
        if (full) {
            // позначити лінію в масиві як заповнену:
            fullLines[y] = true;  // (усі клітинки не чорні)
            cleared++;  // збільшити лічильник
        }
    }

    // Крок 2: Зсув незнищених ліній вниз одним проходом
    // Видаляємо заповнені лінії (fullLines[y] == true), зсуваючи 
    // незнищені вниз. Позиція для запису наступної незнищеної лінії:
    short writeY = cup.height - 1;
    // Починаємо з нижнього рядка, куди зсуваються незнищені лінії.

    for (short readY = cup.height - 1; readY >= 0; readY--) {
        // Перечитуємо рядки знизу вгору для зсуву ліній вниз
        if (!fullLines[readY]) {  // Якщо лінія не заповнена,
            // копіюємо незнищену лінію до позиції writeY, але
            // уникаємо копіювання, якщо позиції збігаються
            if (writeY != readY) {
                // Такий збіг можливий, приміром, для нижньої незнищеної 
                // лінії на першій ітерації або коли після заповнених 
                // ліній іде незнищена на потрібній позиції
                for (short x = 0; x < cup.width; x++) {
                    // Копіюємо клітинки рядка readY у рядок writeY
                    mosaic[writeY][x] = mosaic[readY][x];
                }
            }
            writeY--; // переходимо до наступної позиції для запису вище
        }
        // Заповнені лінії пропускаємо, видаляючи їх.
    }

    /*
    У коді очищення заповнених ліній реалізується так:
    а) пропуск заповнених ліній під час копіювання (вони не потрапляють
    у нижню частину mosaic);
    б) перезапис їх позицій незнищеними лініями (за допомогою зсуву);
    в) очищення верхньої частини поля на кроці 3, що завершує видалення.
    */

    // Крок 3: Очищення верхніх ліній (ті, що вище writeY)
    if (writeY >= 0 && writeY < cup.height) {
        for (short y = 0; y <= writeY; y++) {
            for (short x = 0; x < cup.width; x++) {
                mosaic[y][x] = Color::BLACK;
            }
        }
    }

    // Обов'язкове звільнення пам'яті
    delete[] fullLines;

    // Крок 4: Оновлення рахунку залежно від кількості очищених ліній
    switch (cleared) {
    case 1:
        stats.score += 100;
        break;
    case 2:
        stats.score += 300;
        break;
    case 3:
        stats.score += 700;
        break;
    case 4:
        stats.score += 1500;
        // Початкове очищення області для бонусного повідомлення
        util_ShowText(h, bonusPos.X, bonusPos.Y, "         ", Color::BLACK);
        // Тетрісом в багатьох версіях гри також називається дія, після 
        // якої зникає одразу 4 лінії. Нижче реалізовано відображення 
        // бонусного повідомлення "TETRIS!" за очищення 4 ліній.

        for (short i = 0; i < 4; i++) {  // миготіння чотири рази
            util_ShowText(h, bonusPos.X, bonusPos.Y, "TETRIS!", i % 2 == 0 ?
                Color::YELLOW : Color::BLUE);
            Sleep(TetrisConstants::TETRIS_FLASH_MS / 2);
            // https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleep
            util_ShowText(h, bonusPos.X, bonusPos.Y, "       ", Color::BLACK);
            // Ще одна затримка на 250 мс перед наступною ітерацією
            Sleep(TetrisConstants::TETRIS_FLASH_MS / 2);
        }
        // Виведення статичної емблеми після завершення миготіння
        util_ShowText(h, bonusPos.X, bonusPos.Y, "TETROGRAD", Color::AQUA);
        // видима до наступного очищення 4 ліній або рестарту гри
        break;
    default:
        // Нічого не робимо, якщо не очищено жодної лінії
        break;
    }

    // Крок 5: Оновлення рекордного рахунку
    if (stats.score > stats.highScore) {
        stats.highScore = stats.score;
    }

    // Крок 6: Оновлення рівня гри (наприклад, кожні 1000 очок)
    stats.level = min(stats.score / 1000 + 1, 15);
    // Додано обмеження максимального рівня
}


/**
 * Функція порядкового відображення текстового інтерфейсу керування
 * грою в консолі, використовуючи задану позицію.
 * Параметри функції:
 * - h: дескриптор консолі для виведення тексту керування;
 * - ctrlPos: структура COORD для позиції інтерфейсу керування.
 * Return: немає (void). Виводить текстовий інтерфейс керування.
 * Масиви та посилання не використовуються, крім викликів util_ShowText.
 */
void v4_DrawControls(HANDLE h, COORD ctrlPos)
{
    util_ShowText(h, ctrlPos.X, ctrlPos.Y,
        "   KACZUROWSKI TetraFly Studio", Color::BRIGHT_WHITE);
    util_ShowText(h, ctrlPos.X, ctrlPos.Y + 2, "  ^      Обертання",
        Color::WHITE);
    util_ShowText(h, ctrlPos.X, ctrlPos.Y + 3,
        "<   >    Рух ліворуч / праворуч", Color::WHITE);
    util_ShowText(h, ctrlPos.X, ctrlPos.Y + 4, "  v      М’яке падіння",
        Color::WHITE);
    util_ShowText(h, ctrlPos.X, ctrlPos.Y + 6, "Пробіл   Жорстке падіння",
        Color::WHITE);
    util_ShowText(h, ctrlPos.X, ctrlPos.Y + 8, "Escape   ПАВЗА",
        Color::WHITE);
    util_ShowText(h, ctrlPos.X, ctrlPos.Y + 10, "F1:      Перезапуск гри",
        Color::WHITE);
}


/**
 * Функція порядкового відображення статистики гри (рахунок, рівень,
 * рекорд) у консолі на заданій позиції.
 * Параметри функції:
 * - h: дескриптор консолі для виведення статистики;
 * - stats: константне посилання на структуру GameStats;
 * - statsPos: структура COORD для позиції статистики.
 * Return: немає (void). Виводить статистику гри.
 */
void v4_DrawStatistics(HANDLE h, const GameStats& stats, COORD statsPos)
{
    util_ShowText(h, statsPos.X, statsPos.Y,
        "Рахунок: " + to_string(stats.score), Color::BRIGHT_WHITE);
    util_ShowText(h, statsPos.X, statsPos.Y + 1,
        "Рівень:  " + to_string(stats.level), Color::BRIGHT_WHITE);
    util_ShowText(h, statsPos.X, statsPos.Y + 2,
        "Рекорд:  " + to_string(stats.highScore), Color::BRIGHT_WHITE);
    // https://habr.com/ru/articles/318962/
}


/**
 * Функція малювання ASCII-зображення "стакана" в консолі гри "Тетріс",
 * використовуючи символи для відображення бокових граней та дна.
 * Параметри функції:
 * - h: дескриптор консолі для виведення ASCII-символів граней і дна;
 * - cup: константне посилання на структуру Glass для розмірів і
 * позиції стакана.
 * Return: немає(void). Малює стакан у консолі.
 */
void v4_DrawGlass(HANDLE h, const Glass& cup)
{
    SetConsoleOutputCP(437);  // додано під час тестування гри

    // Малювання вертикальних стінок (бокових граней) стакана
    // Ініціалізація рядка з двох ASCII-символів граней стакана
    // та нуля-термінатора для коректної роботи з рядками
    char line[] = { TetrisConstants::GLASS_EDGE_CHAR,
                    TetrisConstants::GLASS_EDGE_CHAR,
                    '\0'
    };

    for (short y = cup.startY; y < cup.startY + cup.height; y++) {
        // Виведення лівої грані
        util_ShowText(h, cup.startX - 2, y, line, Color::BRIGHT_WHITE);
        // Виведення правої грані
        util_ShowText(h, cup.startX + cup.width, y, line, Color::BRIGHT_WHITE);
    }

    // Оновлення рядка для двох ASCII-символів дна стакана
    line[0] = TetrisConstants::GLASS_BOTTOM_CHAR;
    line[1] = TetrisConstants::GLASS_BOTTOM_CHAR;

    // Малювання нижньої частини (дна) стакана
    for (short x = cup.startX - 2; x < cup.startX + cup.width + 1; x++) {
        util_ShowText(h, x, cup.startY + cup.height, line,
            Color::BRIGHT_WHITE);
    }
}


/**
 * Функція малювання мозаїки гри "Тетроград" всередині "стакана" в
 * консолі, використовуючи ASCII-символи для заповнених і порожніх
 * клітинок із відповідними кольорами, визначеними двовимірним масивом
 * Параметри функції:
 * - h: дескриптор консолі для виведення мозаїки;
 * - mosaic: покажчик на двовимірний масив типу Color** із кольорами
 * клітинок;
 * - cup: константне посилання на структуру Glass для розмірів і
 * позиції мозаїки.
 * Return: немає (void).
 */
void v4_DrawMosaic(HANDLE h, Color** mosaic, const Glass& cup)
{
    SetConsoleOutputCP(437);  // додано під час тестування гри

    // Створення динамічного рядка для буферизації ігрової мозаїки
    char* row = new char[cup.width + 1];  // +1 для нуля-термінатора
    if (row == nullptr) return;  // перевірка на успішне виділення пам'яті

    // Заповнення рядка пробілами
    memset(row, ' ', cup.width); // установлює буфер на вказаний символ
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/memset-wmemset?view=msvc-170
    row[cup.width] = '\0';  // додаємо нуль-термінатор

    // Формування набору рядків висотою cup.height
    for (short y = 0; y < cup.height; y++) {
        // Формування поточного рядка мозаїки
        for (short x = 0; x < cup.width; x++) {
            // Заповнення рядка ASCII-символами: TETROMINO_CHAR для 
            // заповнених клітинок в мозаїці, пробіл для порожніх
            if (mosaic[y][x] == Color::BLACK) row[x] = ' ';
            else row[x] = TetrisConstants::TETROMINO_CHAR;
        }
        // Встановлення позиції для виведення рядка
        COORD pos = { cup.startX, cup.startY + y };
        SetConsoleCursorPosition(h, pos);
        // Виведення рядка, зважаючи на кольори ігрової мозаїки
        for (short x = 0; x < cup.width; x++) {
            SetConsoleTextAttribute(h, static_cast<WORD>(mosaic[y][x]));
            cout << row[x];
        }
    }
    delete[] row;  // звільнення пам'яті після використання рядка
}


/**
 * Перевірка завершення гри
 * Функція перевіряє, чи закінчилася гра, аналізуючи два умови:
 * 1. Чи є заповнені клітинки у верхньому рядку мозаїки
 * (блоки досягли верху).
 * 2. Чи може нове тетраміно бути розміщене в початковій позиції
 * без колізій.
 * Додано параметр current: поточне тетраміно, яке перевіряється
 * на можливість розміщення в початковій позиції.
 * Повертає true, якщо гра закінчилася (верхній ряд заповнений або
 * нове тетраміно не може бути розміщене).
 * Повертає false, якщо гра може продовжуватися.
 */
bool v4_CheckGameOver(Color** mosaic, const Glass& cup,
    const Tetromino& current)
{
    // Чи є заповнені клітинки у верхньому рядку?
    for (short x = 0; x < cup.width; x++) {
        if (mosaic[0][x] != Color::BLACK) {
            return true;  // блоки досягли верхнього краю стакана
        }
    }
    // Чи може поточне тетраміно бути розміщене в початковій позиції?
    if (!v4_CanMove(current, { 0, 0 }, mosaic, cup)) {
        return true;  // неможливо розмістити нове тетраміно
    }
    // Якщо обидві перевірки пройшли, гра продовжується
    return false;
}


/**
 * Функція відображення повідомлення про завершення гри з можливістю
 * вибору дії.
 * Параметри функції:
 * - h: дескриптор консолі для виведення повідомлення;
 * - cup: константне посилання на структуру Glass для позиціонування;
 * - stats: константне посилання на структуру GameStats для
 * відображення рахунку;
 * - restart: посилання на bool, яке встановлюється в true для
 * перезапуску гри (F1) або false для виходу з гри (ESC/інше).
 * Return: немає (void). Модифікує restart для вказівки дії.
 */
void v4_DrawGameOver(HANDLE h, const Glass& cup, const GameStats& stats,
    bool& restart)
{
    // Розташування повідомлення "GAME OVER" над стаканом
    short msgY = cup.startY * 2 / 3;  // визначення координат
    // Ліворуч від стакана з відступом
    short msgX = cup.startX - TetrisConstants::MARGIN_LEFT;
    if (msgX < 0) msgX = 0;  // запобігаємо від’ємним координатам

    // Очищення області перед виведенням повідомлення
    for (short y = msgY; y <= msgY + 10; y++) {
        util_ShowText(h, msgX, y, string(24, ' '), Color::BLACK);
    }

    // Відображення повідомлення про завершення гри та статистики
    util_ShowText(h, msgX, msgY, "       GAME OVER        ", Color::RED);
    util_ShowText(h, msgX, msgY + 2, "Рахунок: " + to_string(stats.score),
        Color::YELLOW);
    util_ShowText(h, msgX, msgY + 3, "Рекорд:  " + to_string(stats.highScore),
        Color::YELLOW);
    util_ShowText(h, msgX, msgY + 5, "F1: Рестарт | ESC: Вихід", Color::GREEN);

    // Очікування вибору гравця
    restart = false;  // за замовчуванням не перезапуск, а вихід з гри
    while (true) {
        if (_kbhit()) {  // перевірка, чи була натиснута клавіша
            // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/kbhit?view=msvc-170
            int key = _getch();  // отримання коду натиснутої клавіші
            // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getch-getwch?view=msvc-170
            // Обробка спеціальних клавіш 
            if (key == 0 || key == 224) key = _getch();

            switch (static_cast<Key>(key)) {
            case Key::RESTART:   // якщо натиснуто F1,
                restart = true;  // встановлюємо перезапуск гри
                return;
            case Key::PAUSE:  // ESC для виходу (тривалої паузи)
                restart = false;
                return;
            default:  // будь-яка інша клавіша також для виходу
                // Виведення інформації про автора
                util_ShowText(h, msgX, msgY + 8,
                    "Розробив студент IT STEP Academy", Color::RED);
                util_ShowText(h, msgX, msgY + 10,
                    "             Руслан КАЧУРОВСЬКИЙ", Color::RED);
                Sleep(TetrisConstants::ABOUT_AUTHOR);
                restart = false;  // встановлюємо вихід із гри
                return;
            }
        }
        // Затримка перед наступною перевіркою вводу
        Sleep(TetrisConstants::SLEEP_DELAY);
    }
}