#ifndef STRINGHELPERS_H
#define STRINGHELPERS_H

#include <stdbool.h>

char* replace_char(char* str, char find, char replace);
int count_occurencies(char* str, char find);
bool contains(char* str, char find);

#endif