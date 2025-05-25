#include "tetrograd_functions.h"
#include <iostream>
#include <cstdlib>   // srand(), rand()
#include <ctime>     // time()
#include <cstdio>    // FILE, fopen_s, fscanf_s, fprintf, fclose
#include <cerrno>    // errno_t
#include <direct.h>  // _mkdir

using namespace std;

// �������� ����� � ����� ����� tetrograd_utils.cpp
static GameStats* activeStats = nullptr;


/**
 * �������� ������� ��� ��������� ������ � ������ ������� �� ������.
 * ������ Color �� ��������� ��� ������������ ���������, �������
 * ������� ��� unsigned char (8 ��) �������� ��� ��� ������� (0�15).
 * ��������� �������:
 * - h: ���������� ������ ��� ��������� ������ � �������;
 * - x: �������� ���� short (X-���������� �������);
 * - y: �������� ���� short (Y-���������� �������);
 * - text: ���������� ��������� �� ��'��� std::string ���
 * ��������� ������;
 * - color: �������� ���� Color (������ ��� ������� ������).
 * Return: ���� (void). ������� �������� ����� � �������.
 * ����������� ���������� ��������� (const string&) ��� ���������
 * �������� ����� ��� ���������.
 */
void util_ShowText(HANDLE h, short x, short y, const string& text,
    Color color)
{
    SetConsoleOutputCP(1251);  // ��� ���������� ����������� ��������
    // ���������� Color �� WORD ���������� ���, ��� ��������� ������� 
    // �������.
    SetConsoleTextAttribute(h, static_cast<WORD>(color));
    // SetConsoleCursorPosition ����������� COORD {x, y} ��� 
    // �������������� �������:
    SetConsoleCursorPosition(h, { x, y });
    cout << text;  // �������� ������� ����� � ������ ������� {x, y}
}


/**
 * ������� ����������� ����������� ������.
 * �������� �������, ��� ��������� �������� ������ ���������
 * ��������, ������� �������� ��� "�����" � "���������". �������
 * ������� �� ������������ ������ ������� ��� �������� ��������,
 * ����������� ���������� ���������� �����, ������������ ���������
 * ���� ������, ������������ ������� ��� ������� ���������� ��
 * ��������� ��������� ��������� �������.
 * ��������� �������:
 * - h - �������� ���� HANDLE (���������� ������), ��������� �����
 * GetStdHandle(STD_OUTPUT_HANDLE).
 * Return: ���� (void). ������� ����� ���� ������ ����� �������� h
 * �� �� ������� ��������.
 * ������� ����������� �� ������� �������� ��� ��������� ������.
 */
void util_SetConsoleOptions(HANDLE h)
{
    // ������������ ������ ������� 1251 ��� �������� ��������
    SetConsoleOutputCP(1251);

    // ����������� ���������� ���������� ����� �� ����� ��������� ����
    srand(static_cast<unsigned>(time(nullptr)));
    // nullptr � ������� ����� UNIX 1 ���� 1970 ����, 00:00:00 UTC

    // ������������ ��������� ���� ������
    SetConsoleTitleA("TETROGRAD");
    // ���� ��������� .exe, � �� � VS, ����������� �� ���������
    // https://learn.microsoft.com/en-us/windows/console/setconsoletitle

    // ������������ ��������� �������
    CONSOLE_CURSOR_INFO cursor = { 100, FALSE };
    // cursor.dwSize = 100;  // ����� 100 (����� ���������� �������)
    // cursor.bVisible = false;  // �������� false (����������)
    // https://learn.microsoft.com/en-us/windows/console/console-cursor-info-str
    // ������������ ����������� ��� ������������ �������
    SetConsoleCursorInfo(h, &cursor);
    // https://learn.microsoft.com/en-us/windows/console/setconsolecursorinfo

    //// ������������ ��������� ������� ������
    //SetConsoleCtrlHandler(util_ConsoleHandler, TRUE);
    //// https://learn.microsoft.com/en-us/windows/console/setconsolectrlhandler
}


/**
* �������� ������� ������ - �������� ������� ��� "���������", ���
* ��������� �������� ���������� �������� � ��� ��������� ����,
* ����� �� Ctrl+C, ������ �������� ������ ��� ��������� �������.
* ������ ����� ���������� �������� � main(): ��������, �� ��������
* ������� (stats.score) �������� �������� ������� (stats.highScore),
* � ���� ���, ������� highScore �� ������ ������ � ���� �� ���������
* util_SaveHighScore. ��� ��������� ���� (CTRL_LOGOFF_EVENT,
* CTRL_SHUTDOWN_EVENT) ������ ������� ��� ��������, ��� �����������
* ����������. ³������� ���� ������ ����� ����������� �� �������
* TRUE, ���� ������ ���������, ��� FALSE ��� �������� �������.
* ��������� �������:
* - signal: ��� ��䳿 (DWORD), �� ����� �� ��� �������.
* Return: BOOL (TRUE ��� FALSE).
*/
BOOL WINAPI util_ConsoleHandler(DWORD signal)
// https://learn.microsoft.com/en-en/windows/console/handlerroutine
{  // �� ����, � ������� ������� ���� ���� ����� ���� ��������
    // �������� ���������� ������������ ������ ������ ��� ���������� 
    // ����� ������ ����� ����������� ��������
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hConsole == INVALID_HANDLE_VALUE) {
        return FALSE;  // ���� ������� ����������
    }

    // ĳ���������: �������� ���������� activeStats
    if (!activeStats) {
        util_ShowText(hConsole, 0, 1, 
            "�������: activeStats == nullptr � util_ConsoleHandler", 
            Color::RED);
        Sleep(3000);
        util_RestoreConsoleState(hConsole);
        ExitProcess(0);
        return TRUE;
    }

    //// ĳ���������: ���� ������� ����������
    //string statsMsg = "Signal: " + to_string(signal) +
    //    ", score=" + to_string(activeStats->score) +
    //    ", highScore=" + to_string(activeStats->highScore) +
    //    ", level=" + to_string(activeStats->level);
    //util_ShowText(hConsole, 0, 1, statsMsg, Color::YELLOW);
    //Sleep(5000);

    // �������� ��� ������� ��� ������ �������� ����� �������:
    switch (signal) {
        // ������� �������, ���������� ������������ � ������
    case CTRL_C_EVENT:  // ������ Ctrl+C ����������� ������� ������
    case CTRL_BREAK_EVENT:  // ������ Ctrl+Break, ���� � ������ Break
        // �������� ����� ���������� ������� � main():
        if (activeStats->score >= activeStats->highScore) {
            activeStats->highScore = activeStats->score;
            util_SaveHighScore(*activeStats);
        }
        // ³��������� ���� ������ ����� �����������
        util_RestoreConsoleState(hConsole);
        // ��������� ������ �� ����� 0 (������ ����������)
        ExitProcess(0);
        // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess
        break;
        // ������� ������� �������� ���� ������ ����������� ������ "X"
    case CTRL_CLOSE_EVENT:  // ������ �������� ������ ������������
        if (activeStats->score >= activeStats->highScore) {
            activeStats->highScore = activeStats->score;
            util_SaveHighScore(*activeStats);
        }
        SetConsoleOutputCP(1251);
        util_RestoreConsoleState(hConsole);
        // ������ �������� (1 �������) ��� �������� ���������� 
        // �������� ���������� � ����. ³����: Windows ������ 
        // ������� CTRL_CLOSE_EVENT �� ~5 ������.
        Sleep(1000);
        ExitProcess(0);  // ����� ��������� ������
        break;
        // ������� ��������� ����: �����, ���������, ����������������
    case CTRL_LOGOFF_EVENT:  // ���������� �������� �� �������
    case CTRL_SHUTDOWN_EVENT: // ��������� ��� ���������������� �������
        // ��� ��������� ���� �������� ������� ��� ��������,
        // ��� ����������� ���������� �����, ����� ���� ������� 
        // �� �������� ��������. �� ����, ���������� ������ �������.
        util_SaveHighScore(*activeStats);
        SetConsoleOutputCP(1251);
        util_RestoreConsoleState(hConsole);
        ExitProcess(0);
        break;
        // ������� �������� �������
    default:
        // ��������� FALSE, ��� ������� ����� �������� ������ ����� 
        // ���������� ��� �������� �� �� �������������
        return FALSE;
    }
    // ��������� TRUE, ��� ��������� �������, �� ������ ������ 
    // ���������, ���������� ��������� ������� ������ �����������
    return TRUE;
}


/**
 * �������� ������� ����������� ���������� ��� ���������.
 * �������� �������� ����� activeStats, ���������� �� ������
 * �������� ��������� stats.
 * ��������� �������:
 * - stats: �������� �� ��������� GameStats, �� ������ ���������� ���.
 */
void util_InitHandlerStats(GameStats* stats)
{
    // ���������� ������������ �������� ���������
    if (!stats) return;
    // ��������� �������� ����� activeStats
    activeStats = stats;
}


/**
 * �������� ������� ���������� ����� ������.
 * ������� ����� ������������ ������, ���������� �����, ���������,
 * � util_SetConsoleOptions. ���� ����� �����, �������� �����������
 * ���� ������, ��������� �������, ������ ������� � ��������� ������.
 * ��������������� ��� ���������� ������ �� ����������� ����� ����
 * ��������� �������� ��� ������� ������.
 * ��������� �������:
 * - h: ���������� ������, ���������, ���������, �� ���������
 * GetStdHandle(STD_OUTPUT_HANDLE).
 * Return: ���� (void).
 */
void util_RestoreConsoleState(HANDLE h)
{
    // ³��������� ������������ ������� ������ (�������-����)
    SetConsoleTextAttribute(h, static_cast<WORD>(Color::BRIGHT_WHITE));

    // ³��������� ��������� ������� -
    CONSOLE_CURSOR_INFO cursor = { 25, TRUE };
    // ����������� ����� 25, ������� ������
    SetConsoleCursorInfo(h, &cursor);

    // ��������� ���������� ��� ����� ������ ������
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    // https://learn.microsoft.com/en-us/windows/console/console-screen-buffer-info-str
    GetConsoleScreenBufferInfo(h, &csbi);
    // https://learn.microsoft.com/en-us/windows/console/getconsolescreenbufferinfo
    // ���������� ������ ������ (������ * ������)
    DWORD size = csbi.dwSize.X * csbi.dwSize.Y;

    // �������� ������: ���������� ��������
    DWORD written;  // lpNumberOfCharsWritten, lpNumberOfAttrsWritten
    FillConsoleOutputCharacter(h, ' ', size, { 0, 0 }, &written);
    // https://learn.microsoft.com/en-us/windows/console/fillconsoleoutputcharacter
    // ³��������� �������� (�������) ��� ��� ������� � �����
    FillConsoleOutputAttribute(h, csbi.wAttributes, size, { 0, 0 }, &written);
    // https://learn.microsoft.com/en-us/windows/console/fillconsoleoutputattribute

    // ���������� ������� � ��������� �������
    SetConsoleCursorPosition(h, { 0, 0 });

    // ³��������� ������ ������� �� ���������� ��������
    SetConsoleOutputCP(GetOEMCP());
    SetConsoleCP(GetOEMCP());
    // https://learn.microsoft.com/en-us/windows/win32/api/winnls/nf-winnls-getoemcp

    // ³��������� ������������ ��������� ������
    SetConsoleTitleA("");
}


/**
 * �������� �������, �� ������ ������ ���� ������.
 * ��������� �������:
 * - h: ���������� ������;
 * - outWidth: ��������� �� ����� ���� short, � ��� ���� ��������
 * ������ ������ � ��������;
 * - outHeight: ��������� �� ����� ���� short, � ��� ���� ��������
 * ������ ������ � ��������.
 * Return: ���� (void).
 * ������ ��������� � ��������� outWidth �� outHeight.
 */
void util_GetConsoleSize(HANDLE h, short& outWidth, short& outHeight)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    // �������� ���������� ��� ����� ������ ������
    GetConsoleScreenBufferInfo(h, &csbi);
    // ���������� ������ �� ������ � ���� ������ ����
    outWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    // ���������� ������ �� ����� � ������ ������ ����
    outHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}


/**
 * ������� ������������ ���������� ������� � �����, ��� ������� ���� 
 * TetroStat.txt � ����� �������, ����� ������ ����� �� ���� ����� 
 * (��������� �������) � ������� ����. ���� ���� �� ����, ������� 
 * ��� ���������� ���������, ����������� -1.
 * Return: int. ��������� ������� ��� -1 � ��� ������� �� ��������� �����.
 */
int util_LoadHighScore()
{
    int highScore = 0;  // �������� �� �������������
    FILE* file = nullptr;
    errno_t readError;
    // https://learn.microsoft.com/en-en/cpp/c-runtime-library/errno-constants?view=msvc-170

    // ³�������� ���� ��� ������� � �������� ���� C:\Tetrograd
    readError = fopen_s(&file, "C:\\Tetrograd\\TetroStat.txt", "r");
    // https://en.cppreference.com/w/c/io/fopen

    // ����������, �� ���� ������ �������
    if (readError == 0 && file != nullptr) {
        // ������� ���� ����� � �����
        // https://en.cppreference.com/w/c/io/fscanf
        if (fscanf_s(file, "%d", &highScore) != 1) {
            highScore = -1;  // ���� ���������� �� �������
        }
        fclose(file);  // ��������� ����, ��� �������� ������ ���'��
    }
    // ���� ���� �� ������� (�� ���� ��� ������� �������), ��������� -1

    return highScore;
}


/**
 * ������� ���������� ���������� ������� � ���� � ���� �� ����� C.
 * ������� ���� Tetrograd, ���� ���� �� ����, ������� ���� 
 * TetroStat.txt � ����� ������, ������ �������� ��������� ������� 
 * (stats.highScore) �� ���� �����. ���� ���� ��� ���� �� ������� 
 * ��������/�������, ������� ����������� ��� �������.
 * ������������� �� ����� � �� ����� �������� ����, ���� ���� � �����.
 * ��������� ��������:
 * - stats: ���������� ��������� �� ��������� GameStats.
 * Return: ���� (void).
 */
void util_SaveHighScore(const GameStats& stats)
{
    FILE* file = nullptr;
    errno_t writeError;

    // ��������� ���� C:\Tetrograd, ���� ���� �� ����
    int dirError = _mkdir("C:\\Tetrograd");
    // https://gist.github.com/sunmeat/06b48b8e00a14f775dcfb949020032d1
    // �� ���� ���� ��������� ���������� ������� Tetrograd_v4.exe

    // ³�������� ���� ��� ������ � ���� C:\Tetrograd
    writeError = fopen_s(&file, "C:\\Tetrograd\\TetroStat.txt", "w");

    // ����������, �� ���� ������ �������
    if (writeError == 0 && file != nullptr) {
        // �������� ��������� ������� � ����
        fprintf(file, "%d", stats.highScore);
        // https://en.cppreference.com/w/c/io/fprintf
        // ����������� ����� ����� � ����
        fflush(file);  // https://en.cppreference.com/w/c/io/fflush
        fclose(file);
    }
    // ���� ���� �� �������� ��� ���� �� �������, 
    // ������� ����������� ��� �������
}