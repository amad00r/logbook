#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

void quote(void) {
    puts(
        "\"logbook is a simple command-line utility for safely storing your thoughts\"\n"
        "~ Amadeu Moya Sardà\n\n"
        "run `logbook help` to learn how to use the tool"
    );
}

void usage(void) {
    puts(
        "Usage: ./logbook [entry|......]\n"
    );

    assert(0 && "add usage of commands");
}

void fatal(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    fputs("[FATAL] ", stderr);
    vfprintf(stderr, msg, args);
    va_end(args);
    putc('\n', stderr);
    exit(EXIT_FAILURE);
}

void info(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    fputs("[INFO] ", stdout);
    vprintf(msg, args);
    va_end(args);
    putchar('\n');
}

#endif