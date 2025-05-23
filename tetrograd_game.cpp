#include "tetrograd_functions.h"
#include <iostream>
#include <conio.h>  // _getch(), _kbhit() � ��� ������ � ����������

using namespace std;


/**
 * ������� ��������� ���� ������ ��������, ����������� ��
 * ������������ �� ������������ � ������ ��������.
 * ��������� �������:
 * � type: �������� ���� Type (������ ���� ��������);
 * - cup: ���������� ��������� �� ��������� Glass ��� �����������
 * ��������.
 * Return: Tetromino. ������� ��������� Tetromino, ������������ �
 * �����, ��������, ������������ �������� � ������.
 * � �������� Tetromino � ����� �������� squares[4] ���� COORD
 * (��������� ����� ����������� ������).
 */
Tetromino v4_CreateTetromino(Type type, const Glass& cup)
{   // �������������� ������� � �������� v4_RotateTetromino()

    // ��������� ������ ������������ ���� ��������:
    Tetromino tm;
    tm.type = type;

    // ����������� ������� �� ��������� ��������� ��������
    switch (type) {  // ������� �� ���� ��������
    case Type::I:
        tm.color = Color::AQUA;   // �����-������ �������� ? LIGHT_AQUA
        tm.squares[0] = { 0, 0 };
        tm.squares[1] = { 1, 0 };  // ����� ���������
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
        tm.color = Color::YELLOW; // �����-������ �����. ? LIGHT_YELLOW
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
        tm.color = Color::PURPLE;  // �����-������ �����.? LIGHT_PURPLE
        tm.squares[0] = { 0, 1 };
        tm.squares[1] = { 1, 1 };  // ����� ���������
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
        tm.squares[1] = { 0, 2 };  // ����� ���������
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
        tm.color = Color::LIGHT_RED;  // � �������� �� ������������!
        tm.squares[0] = { 2, 1 };
        tm.squares[1] = { 2, 2 };  // ����� ���������
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
        tm.color = Color::GREEN;  // �����-������ ��������? LIGHT_GREEN
        tm.squares[0] = { 1, 2 };
        tm.squares[1] = { 0, 2 };  // ����� ���������
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
        tm.color = Color::RED;     // �����-������ �������� ? LIGHT_RED
        tm.squares[0] = { 0, 1 };
        tm.squares[1] = { 1, 1 };  // ����� ���������
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

    // ���������� ������ �������� �� �� X
    short minX = tm.squares[0].X;  // ������� X-����������
    short maxX = tm.squares[0].X;  // ��������� X-����������
    for (short i = 1; i < 4; i++) {
        minX = min(minX, tm.squares[i].X);
        maxX = max(maxX, tm.squares[i].X);
    }
    short tmWidth = maxX - minX + 1;  // ������ ��������

    // �������� �������� �� ���������� � ������ ��������
    tm.offset.X = cup.startX + (cup.width - tmWidth) / 2;
    // ������ ��������� ������� �� �������� �
    tm.offset.Y = 0; // ������� ������ � ��������� ���� ������!

    // ��������� ������������ ��������
    return tm;
}


/**
 * ������� ��������� ������ ������ ��� ��� "���������" � �����������
 * ������, ���� ����������� ���� �.��. �������� ����, �� ����� �������
 * ������ ���� ����� (������ ��� �������� �������).
 * ��������� �������:
 * - cup: ���������� ��������� �� ��������� Glass (height, width,
 * startX, startY) ��� ���������� ������ ������.
 * Return: Color**. ������� �������� �� ����� ��������� (Color**),
 * �� ����� ������� � �� �������� �������� ����� ���� Color
 * (����� ������).
 * �������� ���'��� ���������� ���������� new � �������
 * ��������� ����� v4_DeleteMosaic.
 */
Color** v4_CreateMosaic(const Glass& cup)
{
    // ��������� ����� ��������� ��� ����� ������ ���������
    Color** mosaic = new Color * [cup.height];
    // ������� �� ������ ����������� ������ mosaic:
    for (short i = 0; i < cup.height; i++) {
        // �������� ���'��� ��� �������� � ��������� �����
        mosaic[i] = new Color[cup.width];

        // ����������, �� ����� ������� ���'���
        if (!mosaic[i]) {
            // ������� ������ ��� ��� ��������� ����� ����� �������
            for (short j = 0; j < i; j++) delete[] mosaic[j];
            // ������� ����� ���������, ��� �������� ������ ���'��
            delete[] mosaic;
            return nullptr;
        }

        // ���������� �� ������� ����� �� ������:
        for (short j = 0; j < cup.width; j++)
            mosaic[i][j] = Color::BLACK;
    }

    // ��������� �������� �� ��������� ���������� �����
    return mosaic;  // �������� �� �������� �������
}


/**
 * ������� ��������� ���'�� �� ������ ������.
 * ��������� �������:
 * - mosaic: �������� �� ���������� ����� ���� Color**
 * (������ ������).
 * - height: �������� ���� short (������ ������, �������� �
 * cup.height) ��� �������� �� ������.
 * Return: ���� (void). ������� ������� ���'���, ������� ��� ������.
 * ������� ����������� delete[] ��� ��������� ������� �����,
 * � ���� delete[] ��� ������ ���������.
 */
void v4_DeleteMosaic(Color** mosaic, short height)
{
    // ������� �� ��� ������ ����������� ������ mosaic:
    for (short i = 0; i < height; i++)  // height = cup.height
        // ��������� ������, ������� ��� �������� � ��������� �����
        delete[] mosaic[i];
    // ��������� ������, ������� ��� ������ ��������� �� �����
    delete[] mosaic;
}


/**
 * ������� ��������� �������� �� 90 ������� �� ������������ �������
 * ������� ������ ���������. ����������� ������� "wall kicks" ���
 * ��������� ����� � �������� �������, ���� ��� ������ ��������.
 * ���� ��������� ��������� ����� ���糿, �������� ���������� �
 * ����������� ����.
 * ��������� �������:
 * - tm: ��������� �� ��������� Tetromino, �� ������������
 * �� ��� ���������;
 * - mosaic: ���������� ����� Color** ��� �������� �����;
 * - cup: ��������� Glass ��� �������� ��� �������.
 * Return: ���� (void). ������� ����� ���������� �� ���� tm � ���
 * �������� ��������� ��� ������ ��� ���, ���� ��������� ���������.
 * ����������� ��������� (Tetromino&) ��� ���� ��������� tm.
 * ����������� ��������� ��������� ����� kicks[7][2] ��� �����
 * ("wall kicks") � ���������� ����� newSquares[4] ��� ����� ���������.
 */
void v4_RotateTetromino(Tetromino& tm, Color** mosaic, const Glass& cup)
{  // �������������� ������� � �������� v4_CreateTetromino()

    // �������� O �� ���������� ����� ���� ������� (������� 2x2):
    if (tm.type == Type::O) return;

    // ��������� ����� ��������� (������ ������� � ����� tm.squares):
    COORD center = tm.squares[1];  // ��� ��� �������� (��� O)
    // ��� �������� ����� ��������� ����� ��� ����������� ���������
    // ����� ��������� �������� �������� ���� ���������
    COORD newSquares[4];

    // ���������� ��� ���������� �������� ���� ��������� 
        // https://uk.wikipedia.org/wiki/�������_��������
        // [ 0  -1
        //   1   0 ]
    // �� 90 ������� �� ������������ �������:
    for (short i = 0; i < 4; i++) {
        short dx = tm.squares[i].X - center.X; // �������� ���� �� X
        short dy = tm.squares[i].Y - center.Y; // �������� ���� �� Y
        newSquares[i].X = center.X + dy;       // ���� ���������� X
        newSquares[i].Y = center.Y - dx;       // ���� ���������� Y
    }

    // ����������� ����� ����� ("wall kicks") ��� ��������� �����.
    // ����� ���� {dx, dy} ������� ������ ����� �������� ���� ���������:
    short kicks[7][2] = {  // "wall kicks" ����������� ��� �������� I
        { 0, 0 },   // ��� �����
        { -1, 0 },  // ���� �� 1 ������� ������
        { 1, 0 },   // ���� �� 1 ������� ��������
        { -2, 0 },  // ����� �� 2 ������� ������
        { 2, 0 },   // ����� �� 2 ������� ��������
        { 0, 1 },   // ���� �� 1 ������� �����
        { 0, -1 }   // ���� �� 1 ������� ����
    };

    // ���������� �� ������ ����� � ������ kicks:
    for (auto& k : kicks) {
        // ������� ��������� ��� ��������,
        bool ok = true;  // �� ������� ��������� � �������� ������

        // ���������� ����� ������� �������� ���� ���������:
        for (short j = 0; j < 4; j++) {
            // �������� ���������� ������ �������� � ������ ��������
            short x = newSquares[j].X + tm.offset.X + k[0];
            short y = newSquares[j].Y + tm.offset.Y + k[1];

            // ����������, �� �������� ������� �� ��� ���������:
            if (x < cup.startX || x >= cup.startX + cup.width ||
                y >= cup.startY + cup.height) {
                ok = false;  // ���� ���� �����, ��������� ���������
                break;
            }

            // ����������, �� �� ������������ ������� �� ��������� 
            // ��������� � ������ ������. ���� ������� ����������� �
            // ����� ������ ��������� � ������� �� �������, � �����.
            if (y >= cup.startY &&
                mosaic[y - cup.startY][x - cup.startX] != Color::BLACK) {
                ok = false;  // � ���� � �����, ��������� ���������
                break;
            }
        }

        // ���� ��������� ������� � �������� ������ (���� �����),
        // ����������� ��� ���������� �������� �� ��������� �����:
        if (ok) {
            for (short j = 0; j < 4; j++) {
                // ��������� ������� ���������� ��������
                tm.squares[j] = newSquares[j];
            }
            tm.offset.X += k[0];  // ��������� ���� �� X
            tm.offset.Y += k[1];  // ��������� ���� �� Y

            return;  // ��������� ������� ���� �������� ���������
        }
    }

    // ���� ����� ���� �� �������� ��������� (�� ������ �������� �� 
    // �����), ������� ����������� ��� ��� � �������� 
    // (tm ���������� � ����������� ����).
}


/**
 * ������� ��������, �� ���� �������� ������������ �� ������ �������
 * {delta.X, delta.Y} ��� ������ �� ��� ������ ��������� �� ���
 * ����� � ������ ��������.
 * ��������� �������:
 * - tm: ���������� ��������� �� ��������� Tetromino (���, ����������
 * ��������, ����, ����);
 * - delta: ��������� COORD �� ������� �� X �� Y (delta.X, delta.Y)
 * ��� ����;
 * - mosaic: �������� �� ���������� ����� ���� Color**
 * (������ ������);
 * - cup: ���������� ��������� �� ��������� Glass ��� �������� ���.
 * Return: bool. ������� true, ���� ���������� ������� ��� �����,
 * � false, ���� � ���糿 ��� ����� �� ��� �������.
 * � �������� Tetromino � ��������� ����� squares[4].
 */
bool v4_CanMove(const Tetromino& tm, COORD delta, Color** mosaic,
    const Glass& cup)
{  // ���. �������� �� ������� v4_RotateTetromino()
    for (short i = 0; i < 4; i++) {
        // ��� �������� ���������� �������� ���� ����������:
        short x = tm.squares[i].X + tm.offset.X + delta.X;
        short y = tm.squares[i].Y + tm.offset.Y + delta.Y;

        if (x < cup.startX || x >= cup.startX + cup.width ||
            y >= cup.startY + cup.height)  // �������� �� ���?
            return false;  // ��� ��� ����������

        // �� ����, mosaic � ������ ������� ������ ��������� cup
        if (y >= cup.startY && mosaic[y - cup.startY][x - cup.startX]
            != Color::BLACK)  // � �����?
            return false;  // ����, ��� ����������
    }

    // ���� ����� ������� �� �������� ����� ��� �� ������ �� ���,
    return true;  // ��� ��������!
}


/**
 * ������� ���������� �������� � ������ ������, ��� ���� ��������
 * �������� �� ������. ϳ��� �������� �������� ��� �������� ������
 * �� ����� �� ���� ��������. ������� �� ������� ��������, ���
 * ����� ����� ������ ������!
 * ��������� �������:
 * - tm: ���������� ��������� �� ��������� Tetromino, �������� ���
 * ����������;
 * - mosaic: �������� �� ���������� ����� ���� Color** ��� ���������
 * �������� ��������;
 * - cup: ���������� ��������� �� ��������� Glass ��� ��������� �
 * ������.
 * Return: ���� (void). �������� mosaic, ������� �������� ��������.
 * � �������� Tetromino � ��������� ����� squares[4].
 */
void v4_FixTetromino(const Tetromino& tm, Color** mosaic, const Glass& cup)
{
    for (short i = 0; i < 4; i++) {
        // ���������� �������� � ������ ��������� ������ mosaic:
        short x = tm.squares[i].X + tm.offset.X - cup.startX;
        short y = tm.squares[i].Y + tm.offset.Y - cup.startY;

        if (y >= 0 && y < cup.height && x >= 0 && x < cup.width)
            // �������� �������� � ������ �������� ��������
            mosaic[y][x] = tm.color;
    }
}


/**
 * ������� ��� �������� �������� � ������ � ��������� ��
 * �������������.
 * ��������� �������:
 * - h: ���������� ������ ��� �������� ��������;
 * - tm: ���������� ��������� �� ��������� Tetromino, �� ��������
 * ���������� ��������.
 * Return: ���� (void). ����� �������� � ������.
 * � �������� Tetromino � ��������� ����� squares[4].
 */
void v4_EraseTetromino(HANDLE h, const Tetromino& tm)
{
    // ������������ ������ ������� 437 (OEM US) ��� ���������� 
    // ����������� ��������������� �������
    SetConsoleOutputCP(437);  // ������ �� ��� ���������� ���

    // ������������ ������� ������� ������ ��� "��������"
    SetConsoleTextAttribute(h, static_cast<WORD>(Color::BLACK));
    // (�������� � ����� ������)

    for (short i = 0; i < 4; i++) {
        COORD pos;  // �������� ���������� �������� ��������
        pos.X = tm.squares[i].X + tm.offset.X;
        pos.Y = tm.squares[i].Y + tm.offset.Y;

        // ���������� ������� � ��������� �������
        SetConsoleCursorPosition(h, pos);
        // ��������� ������ ��� ����� �������� ��������
        cout << ' ';  // (����� ��������)
    }
}


/**
 * ������� ��� ����������� �������� �� ������ � ������ �������,
 * ��������� ���� ������� �������. ������� �� ������� ��������, ���
 * �������� ������� �������� �� �������.
 * ��������� �������:
 * - h: ���������� ������ ��� ��������� ��������;
 * - tm: ���������� ��������� �� ��������� Tetromino (�����, ����,
 * ����������);
 * - position: ��������� COORD ��� ������ ������� ���������;
 * Return: ���� (void). ����� �������� � ������ �������.
 * � �������� Tetromino � ��������� ����� squares[4].
 */
void v4_ShowTetrominoAt(HANDLE h, const Tetromino& tm, COORD position)
{
    SetConsoleOutputCP(437);  // ������ �� ��� ���������� ���
    // ������������ ������� ������ ��� ��������� ��������
    SetConsoleTextAttribute(h, static_cast<WORD>(tm.color));

    for (short i = 0; i < 4; i++) {
        // ���������� ������� ������� �������� �� ������:
        COORD coutPosChar;
        coutPosChar.X = position.X + tm.squares[i].X;
        coutPosChar.Y = position.Y + tm.squares[i].Y;

        SetConsoleCursorPosition(h, coutPosChar);
        cout << TetrisConstants::TETROMINO_CHAR;  // ������ ��������
    }
}


/**
 * ������� ��� ��������� �������� �� ������ � ���� ������� �������,
 * �������������� �������. ������� ������� v4_ShowTetrominoAt(), �� �
 * �������� �������� ���������, �� ��������, ��� ������� tm.offset.
 * ��������� �������:
 * - h: ���������� ������ ��� ��������� ��������� ��������;
 * - tm: ���������� ��������� �� ��������� Tetromino �� ������������
 * �� ������.
 * Return: ���� (void). ����� ������� �������� � ���� �������.
 */
void v4_DrawCurrentTetromino(HANDLE h, const Tetromino& tm)
{
    COORD position = { tm.offset.X, tm.offset.Y };
    v4_ShowTetrominoAt(h, tm, position);
}


/**
 * ������� ��� ��������� ���������� �������� � ������ ������� ��
 * ������. ����� �������, �� ���� ����������� ��������, � �������
 * ������� v4_ShowTetrominoAt() ��� ��������� ���������� ��������.
 * ��������� �������:
 * - h: ���������� ������ ��� ��������� ���������� ��������;
 * - next: ���������� ��������� �� ��������� Tetromino
 * (�������� ��������);
 * - nextPos: ��������� COORD ��� ������� ���������.
 * Return: ���� (void). ����� ������� � ����� �������� ��������.
 * � next � ��������� ����� squares[4].
 */
void v4_DrawNextTetromino(HANDLE h, const Tetromino& next, COORD nextPos)
{
    // �������� ������ 5x5, �� ���� ������������ �������� ��������.
    // �� ���������, ��� �������� �������� �������� ��� ������� �������.
    for (short y = 0; y < 5; y++) {
        for (short x = 0; x < 5; x++) {
            // ��������� ������ � ������ ������� � �������� BLACK, 
            // ��� ����������� ������ ���
            util_ShowText(h, nextPos.X + x, nextPos.Y + y, " ", Color::BLACK);
        }
    }

    // ��������� ���������� �������� � ��������� ������� �� ������
    v4_ShowTetrominoAt(h, next, nextPos);
}


/**
 * ������� �������� �� �������� ���������� ���, ��������� ����������
 * (�������, �����, ������), � ����� ����������� ��������� ���������
 * � ������� �������� �� ������ ������� � ��� �������� 4 ���.
 * ��������� �������:
 * - h: ���������� ������ ��� ��������� ��������� �����������;
 * - mosaic: �������� �� ���������� ����� ���� Color** ���
 * �������� ���;
 * - cup: ���������� ��������� �� ��������� Glass ��� ������ ������;
 * - stats: ��������� �� ��������� GameStats ��� ��������� ����������;
 * - bonusPos: ��������� COORD ��� ������� ����������� "TETRIS!".
 * Return: ���� (void). �������� mosaic (����� ��) � stats.
 * ����������� �������� �������� ����� fullLines ���� bool* ���
 * ���������� ���������� ��� (����������� ����� delete[]).
 */
void v4_CheckLines(HANDLE h, Color** mosaic, const Glass& cup,
    GameStats& stats, COORD bonusPos)
{
    short cleared = 0;  // �������� �������� ���
    // ����� ��� ���������� ���������� ��� (������������ false):
    bool* fullLines = new bool[cup.height]();  // +1 ��������� �����

    // ���� 1: ��������� ��� ���������� ���
    for (short y = 0; y < cup.height; y++) {
        bool full = true;  // ����������, �� ��� ���������
        for (short x = 0; x < cup.width; x++) {
            if (mosaic[y][x] == Color::BLACK) {
                full = false;
                break;  // ���� � ����� �������, �� ��� �� ���������
            }
        }
        if (full) {
            // ��������� ��� � ����� �� ���������:
            fullLines[y] = true;  // (�� ������� �� ����)
            cleared++;  // �������� ��������
        }
    }

    // ���� 2: ���� ���������� ��� ���� ����� ��������
    // ��������� �������� �� (fullLines[y] == true), �������� 
    // �������� ����. ������� ��� ������ �������� ��������� ��:
    short writeY = cup.height - 1;
    // �������� � �������� �����, ���� ���������� �������� ��.

    for (short readY = cup.height - 1; readY >= 0; readY--) {
        // ���������� ����� ����� ����� ��� ����� ��� ����
        if (!fullLines[readY]) {  // ���� ��� �� ���������,
            // ������� ��������� ��� �� ������� writeY, ���
            // ������� ���������, ���� ������� ���������
            if (writeY != readY) {
                // ����� ��� ��������, �������, ��� ������ ��������� 
                // �� �� ������ �������� ��� ���� ���� ���������� 
                // ��� ��� ��������� �� ������� �������
                for (short x = 0; x < cup.width; x++) {
                    // ������� ������� ����� readY � ����� writeY
                    mosaic[writeY][x] = mosaic[readY][x];
                }
            }
            writeY--; // ���������� �� �������� ������� ��� ������ ����
        }
        // �������� �� ����������, ��������� ��.
    }

    /*
    � ��� �������� ���������� ��� ���������� ���:
    �) ������� ���������� ��� �� ��� ��������� (���� �� �����������
    � ����� ������� mosaic);
    �) ��������� �� ������� ����������� ����� (�� ��������� �����);
    �) �������� ������� ������� ���� �� ����� 3, �� ������� ���������.
    */

    // ���� 3: �������� ������ ��� (�, �� ���� writeY)
    if (writeY >= 0 && writeY < cup.height) {
        for (short y = 0; y <= writeY; y++) {
            for (short x = 0; x < cup.width; x++) {
                mosaic[y][x] = Color::BLACK;
            }
        }
    }

    // ����'������ ��������� ���'��
    delete[] fullLines;

    // ���� 4: ��������� ������� ������� �� ������� �������� ���
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
        // ��������� �������� ������ ��� ��������� �����������
        util_ShowText(h, bonusPos.X, bonusPos.Y, "         ", Color::BLACK);
        // ������� � �������� ������ ��� ����� ���������� ��, ���� 
        // ��� ����� ������ 4 ��. ����� ���������� ����������� 
        // ��������� ����������� "TETRIS!" �� �������� 4 ���.

        for (short i = 0; i < 4; i++) {  // �������� ������ ����
            util_ShowText(h, bonusPos.X, bonusPos.Y, "TETRIS!", i % 2 == 0 ?
                Color::YELLOW : Color::BLUE);
            Sleep(TetrisConstants::TETRIS_FLASH_MS / 2);
            // https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleep
            util_ShowText(h, bonusPos.X, bonusPos.Y, "       ", Color::BLACK);
            // �� ���� �������� �� 250 �� ����� ��������� ���������
            Sleep(TetrisConstants::TETRIS_FLASH_MS / 2);
        }
        // ��������� �������� ������� ���� ���������� ��������
        util_ShowText(h, bonusPos.X, bonusPos.Y, "TETROGRAD", Color::AQUA);
        // ������ �� ���������� �������� 4 ��� ��� �������� ���
        break;
    default:
        // ͳ���� �� ������, ���� �� ������� ����� ��
        break;
    }

    // ���� 5: ��������� ���������� �������
    if (stats.score > stats.highScore) {
        stats.highScore = stats.score;
    }

    // ���� 6: ��������� ���� ��� (���������, ���� 1000 ����)
    stats.level = min(stats.score / 1000 + 1, 15);
    // ������ ��������� ������������� ����
}


/**
 * ������� ����������� ����������� ���������� ���������� ���������
 * ���� � ������, �������������� ������ �������.
 * ��������� �������:
 * - h: ���������� ������ ��� ��������� ������ ���������;
 * - ctrlPos: ��������� COORD ��� ������� ���������� ���������.
 * Return: ���� (void). �������� ��������� ��������� ���������.
 * ������ �� ��������� �� ����������������, ��� ������� util_ShowText.
 */
void v4_DrawControls(HANDLE h, COORD ctrlPos)
{
    util_ShowText(h, ctrlPos.X, ctrlPos.Y,
        "   KACZUROWSKI TetraFly Studio", Color::BRIGHT_WHITE);
    util_ShowText(h, ctrlPos.X, ctrlPos.Y + 2, "  ^      ���������",
        Color::WHITE);
    util_ShowText(h, ctrlPos.X, ctrlPos.Y + 3,
        "<   >    ��� ������ / ��������", Color::WHITE);
    util_ShowText(h, ctrlPos.X, ctrlPos.Y + 4, "  v      ̒��� ������",
        Color::WHITE);
    util_ShowText(h, ctrlPos.X, ctrlPos.Y + 6, "�����   ������� ������",
        Color::WHITE);
    util_ShowText(h, ctrlPos.X, ctrlPos.Y + 8, "Escape   �����",
        Color::WHITE);
    util_ShowText(h, ctrlPos.X, ctrlPos.Y + 10, "F1:      ���������� ���",
        Color::WHITE);
}


/**
 * ������� ����������� ����������� ���������� ��� (�������, �����,
 * ������) � ������ �� ������ �������.
 * ��������� �������:
 * - h: ���������� ������ ��� ��������� ����������;
 * - stats: ���������� ��������� �� ��������� GameStats;
 * - statsPos: ��������� COORD ��� ������� ����������.
 * Return: ���� (void). �������� ���������� ���.
 */
void v4_DrawStatistics(HANDLE h, const GameStats& stats, COORD statsPos)
{
    util_ShowText(h, statsPos.X, statsPos.Y,
        "�������: " + to_string(stats.score), Color::BRIGHT_WHITE);
    util_ShowText(h, statsPos.X, statsPos.Y + 1,
        "г����:  " + to_string(stats.level), Color::BRIGHT_WHITE);
    util_ShowText(h, statsPos.X, statsPos.Y + 2,
        "������:  " + to_string(stats.highScore), Color::BRIGHT_WHITE);
    // https://habr.com/ru/articles/318962/
}


/**
 * ������� ��������� ASCII-���������� "�������" � ������ ��� "�����",
 * �������������� ������� ��� ����������� ������� ������ �� ���.
 * ��������� �������:
 * - h: ���������� ������ ��� ��������� ASCII-������� ������ � ���;
 * - cup: ���������� ��������� �� ��������� Glass ��� ������ �
 * ������� �������.
 * Return: ����(void). ����� ������ � ������.
 */
void v4_DrawGlass(HANDLE h, const Glass& cup)
{
    SetConsoleOutputCP(437);  // ������ �� ��� ���������� ���

    // ��������� ������������ ����� (������� ������) �������
    // ����������� ����� � ���� ASCII-������� ������ �������
    // �� ����-���������� ��� �������� ������ � �������
    char line[] = { TetrisConstants::GLASS_EDGE_CHAR,
                    TetrisConstants::GLASS_EDGE_CHAR,
                    '\0'
    };

    for (short y = cup.startY; y < cup.startY + cup.height; y++) {
        // ��������� ��� ����
        util_ShowText(h, cup.startX - 2, y, line, Color::BRIGHT_WHITE);
        // ��������� ����� ����
        util_ShowText(h, cup.startX + cup.width, y, line, Color::BRIGHT_WHITE);
    }

    // ��������� ����� ��� ���� ASCII-������� ��� �������
    line[0] = TetrisConstants::GLASS_BOTTOM_CHAR;
    line[1] = TetrisConstants::GLASS_BOTTOM_CHAR;

    // ��������� ������ ������� (���) �������
    for (short x = cup.startX - 2; x < cup.startX + cup.width + 1; x++) {
        util_ShowText(h, x, cup.startY + cup.height, line,
            Color::BRIGHT_WHITE);
    }
}


/**
 * ������� ��������� ������ ��� "���������" �������� "�������" �
 * ������, �������������� ASCII-������� ��� ���������� � �������
 * ������� �� ���������� ���������, ����������� ���������� �������
 * ��������� �������:
 * - h: ���������� ������ ��� ��������� ������;
 * - mosaic: �������� �� ���������� ����� ���� Color** �� ���������
 * �������;
 * - cup: ���������� ��������� �� ��������� Glass ��� ������ �
 * ������� ������.
 * Return: ���� (void).
 */
void v4_DrawMosaic(HANDLE h, Color** mosaic, const Glass& cup)
{
    SetConsoleOutputCP(437);  // ������ �� ��� ���������� ���

    // ��������� ���������� ����� ��� ����������� ������ ������
    char* row = new char[cup.width + 1];  // +1 ��� ����-����������
    if (row == nullptr) return;  // �������� �� ������ �������� ���'��

    // ���������� ����� ��������
    memset(row, ' ', cup.width); // ���������� ����� �� �������� ������
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/memset-wmemset?view=msvc-170
    row[cup.width] = '\0';  // ������ ����-���������

    // ���������� ������ ����� ������� cup.height
    for (short y = 0; y < cup.height; y++) {
        // ���������� ��������� ����� ������
        for (short x = 0; x < cup.width; x++) {
            // ���������� ����� ASCII-���������: TETROMINO_CHAR ��� 
            // ���������� ������� � ������, ����� ��� �������
            if (mosaic[y][x] == Color::BLACK) row[x] = ' ';
            else row[x] = TetrisConstants::TETROMINO_CHAR;
        }
        // ������������ ������� ��� ��������� �����
        COORD pos = { cup.startX, cup.startY + y };
        SetConsoleCursorPosition(h, pos);
        // ��������� �����, �������� �� ������� ������ ������
        for (short x = 0; x < cup.width; x++) {
            SetConsoleTextAttribute(h, static_cast<WORD>(mosaic[y][x]));
            cout << row[x];
        }
    }
    delete[] row;  // ��������� ���'�� ���� ������������ �����
}


/**
 * �������� ���������� ���
 * ������� ��������, �� ���������� ���, ��������� ��� �����:
 * 1. �� � �������� ������� � ��������� ����� ������
 * (����� ������� �����).
 * 2. �� ���� ���� �������� ���� �������� � ��������� �������
 * ��� �����.
 * ������ �������� current: ������� ��������, ��� ������������
 * �� ��������� ��������� � ��������� �������.
 * ������� true, ���� ��� ���������� (������ ��� ���������� ���
 * ���� �������� �� ���� ���� ��������).
 * ������� false, ���� ��� ���� ��������������.
 */
bool v4_CheckGameOver(Color** mosaic, const Glass& cup,
    const Tetromino& current)
{
    // �� � �������� ������� � ��������� �����?
    for (short x = 0; x < cup.width; x++) {
        if (mosaic[0][x] != Color::BLACK) {
            return true;  // ����� ������� ��������� ���� �������
        }
    }
    // �� ���� ������� �������� ���� �������� � ��������� �������?
    if (!v4_CanMove(current, { 0, 0 }, mosaic, cup)) {
        return true;  // ��������� ��������� ���� ��������
    }
    // ���� ����� �������� �������, ��� ������������
    return false;
}


/**
 * ������� ����������� ����������� ��� ���������� ��� � ���������
 * ������ 䳿.
 * ��������� �������:
 * - h: ���������� ������ ��� ��������� �����������;
 * - cup: ���������� ��������� �� ��������� Glass ��� ��������������;
 * - stats: ���������� ��������� �� ��������� GameStats ���
 * ����������� �������;
 * - restart: ��������� �� bool, ��� �������������� � true ���
 * ����������� ��� (F1) ��� false ��� ������ � ��� (ESC/����).
 * Return: ���� (void). �������� restart ��� ������� 䳿.
 */
void v4_DrawGameOver(HANDLE h, const Glass& cup, const GameStats& stats,
    bool& restart)
{
    // ������������ ����������� "GAME OVER" ��� ��������
    short msgY = cup.startY * 2 / 3;  // ���������� ���������
    // ˳����� �� ������� � ��������
    short msgX = cup.startX - TetrisConstants::MARGIN_LEFT;
    if (msgX < 0) msgX = 0;  // ��������� �䒺���� �����������

    // �������� ������ ����� ���������� �����������
    for (short y = msgY; y <= msgY + 10; y++) {
        util_ShowText(h, msgX, y, string(24, ' '), Color::BLACK);
    }

    // ³���������� ����������� ��� ���������� ��� �� ����������
    util_ShowText(h, msgX, msgY, "       GAME OVER        ", Color::RED);
    util_ShowText(h, msgX, msgY + 2, "�������: " + to_string(stats.score),
        Color::YELLOW);
    util_ShowText(h, msgX, msgY + 3, "������:  " + to_string(stats.highScore),
        Color::YELLOW);
    util_ShowText(h, msgX, msgY + 5, "F1: ������� | ESC: �����", Color::GREEN);

    // ���������� ������ ������
    restart = false;  // �� ������������� �� ����������, � ����� � ���
    while (true) {
        if (_kbhit()) {  // ��������, �� ���� ��������� ������
            // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/kbhit?view=msvc-170
            int key = _getch();  // ��������� ���� ��������� ������
            // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getch-getwch?view=msvc-170
            // ������� ����������� ����� 
            if (key == 0 || key == 224) key = _getch();

            switch (static_cast<Key>(key)) {
            case Key::RESTART:   // ���� ��������� F1,
                restart = true;  // ������������ ���������� ���
                return;
            case Key::PAUSE:  // ESC ��� ������ (������� �����)
                restart = false;
                return;
            default:  // ����-��� ���� ������ ����� ��� ������
                // ��������� ���������� ��� ������
                util_ShowText(h, msgX, msgY + 8,
                    "�������� ������� IT STEP Academy", Color::RED);
                util_ShowText(h, msgX, msgY + 10,
                    "             ������ ������������", Color::RED);
                Sleep(TetrisConstants::ABOUT_AUTHOR);
                restart = false;  // ������������ ����� �� ���
                return;
            }
        }
        // �������� ����� ��������� ��������� �����
        Sleep(TetrisConstants::SLEEP_DELAY);
    }
}