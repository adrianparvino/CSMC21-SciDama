#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#define OS_Windows
#endif

int pdn_to_index(int pdn);
int index_to_pdn(int index);

bool is_valid_position(int position);

void clear_screen(void);
#endif