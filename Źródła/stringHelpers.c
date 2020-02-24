#include <string.h>
#include <stdbool.h>

#include "stringHelpers.h"

char* replace_char(char* str, char find, char replace)
{
    char *current_pos = strchr(&str, find);
	for (char* p = current_pos; (current_pos = strchr(str, find)) != NULL; *current_pos = replace);
    return str;
}

int count_occurencies(char* str, char find)
{
    int counter = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == find)
            counter += 1;
    }
    
    return counter;
}

bool contains(char* str, char find)
{
    if (strchr(str, find) != NULL)
        return true;

    return false;
}