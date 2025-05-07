#include "utils.h"

#include <stdio.h>

// https://stackoverflow.com/a/47406222
#ifdef OS_Windows
void clear_screen(void) {
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD topLeft = {0, 0};
    DWORD dwCount, dwSize;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOutput, &csbi);
    dwSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hOutput, 0x20, dwSize, topLeft, &dwCount);
    FillConsoleOutputAttribute(hOutput, 0x07, dwSize, topLeft, &dwCount);
    SetConsoleCursorPosition(hOutput, topLeft);
}
#else
void clear_screen(void) { printf("\x1B[2J"); }
#endif /* __unix__ */

int pdn_to_index(int pdn) {
    --pdn;
    int i = pdn / 4;
    int j = 2 * (pdn % 4) + (i + 1) % 2;

    return 8 * i + j;
}

int index_to_pdn(int index) {
    int i = index / 8;
    int j = index % 8;

    j /= 2;

    return 4 * i + j + 1;
}

bool is_valid_position(int position) {
    if (position < 0 || position >= 64)
        return false;

    int i = position / 8;
    int j = position % 8;

    return (i % 2) != (j % 2);
}

