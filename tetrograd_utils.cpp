#include "tetrograd_functions.h"
#include <iostream>
#include <cstdlib>  // srand(), rand()
#include <ctime>    // time()

using namespace std;


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
{
    // ��������� �������� �� GameStats, ���������� �������� �������
    static GameStats* activeStats = nullptr;

    // �������� ���������� ������������ ������ ������ ��� ���������� 
    // ����� ������ ����� ����������� ��������
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hConsole == INVALID_HANDLE_VALUE) {
        return FALSE;  // ���� ������� ����������
    }

    // �������� �� nullptr ����� �������������
    if (!activeStats) {
        util_RestoreConsoleState(hConsole);
        return FALSE;
    }

    // �������� ��� ������� ��� ������ �������� ����� �������:
    switch (signal) {
        // ������� �������, ���������� ������������ � ������
    case CTRL_C_EVENT:  // ������ Ctrl+C ����������� ������� ������
    case CTRL_BREAK_EVENT:  // ������ Ctrl+Break, ���� � ������ Break
        // �������� ����� ���������� ������� � main():
        if (activeStats->score > activeStats->highScore) {
            activeStats->highScore = activeStats->score;
            util_SaveHighScore(*activeStats);
        }
        SetConsoleOutputCP(1251);
        // ³��������� ���������� ���� ������
        util_RestoreConsoleState(hConsole);
        // ��������� ������ �� ����� 0 (������ ����������)
        ExitProcess(0);
        // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess
        break;
        // ������� ������� �������� ���� ������ ����������� ������ "X"
    case CTRL_CLOSE_EVENT:  // ������ �������� ������ ������������
        if (activeStats->score > activeStats->highScore) {
            activeStats->highScore = activeStats->score;
            util_SaveHighScore(*activeStats);
        }
        SetConsoleOutputCP(1251);
        util_RestoreConsoleState(hConsole);
        // ������ �������� �������� (0.5 �������) ��� �������� 
        // ���������� �������� ���������� � ����. ³����: Windows 
        // ������ ������� CTRL_CLOSE_EVENT �� ~5 ������.
        Sleep(500);
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
 * �������� �������� �� ��������� GameStats, ���������� ���� ������
 * �������� ��������� stats. ������� ������� ���'��.
 * ��������� �������:
 * - stats: �������� �� ��������� GameStats, �� ������ ���������� ���;
 * - activeStats: �������� �������� �� ��������� GameStats, ��
 * ���� ��������, ��������� ������ stats ��� ���������� ������������.
 */
void util_InitHandlerStats(GameStats* stats, GameStats** activeStats)
{
    // ���������� ������������ �������� ���������
    if (!stats || !activeStats) return;
    // ����� ��������� ����� �������� �� ��������
    *activeStats = stats;
    // ����� activeStats ����� �� �� � ������� �����, �� � stats.
    // �������� �������� ������ �������� ��������� ���� ������ �������, 
    // �� ������� ��� ��������� ������ ���.
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


// ������������ ���������� �������
int util_LoadHighScore()  // ���� ���������� ���������� ��������
{
    // ������� �������� ��� ��������, ��� �� ������ ������ ��
    return 0;
}


// ���������� ���������� �������
void util_SaveHighScore(const GameStats& stats)  // ���� ���������� ��
{
    // ������� �������� ��� ��������, ��� �� ������ ������ ��
}