#ifndef STRIP_H
#define STRIP_H

#include <string.h>
#include <stdio.h>

char *strip(char *str) {
    char *stripped_str = str;
    char c = *stripped_str;
    while (c == ' ' || c == '\n' || c == '\t') c = *++stripped_str;
    unsigned int size = strlen(stripped_str);
    while (size > 0 && ((c = stripped_str[size - 1]) == ' ' || c == '\n' || c == '\t' || c == EOF)) --size;
    if (size > 0) stripped_str[size] = '\0';
    return stripped_str;
}

#endif