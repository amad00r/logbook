#ifndef PATH_H
#define PATH_H

#include <stdlib.h>
#include <string.h>
#include <errno.h>

char *get_logbook_path(const char *name) {
    char *home = getenv("HOME");
    int home_path_len = strlen(home);
    char *path = malloc(sizeof(char)*(home_path_len + strlen(name) + 8)); // len("/.logbook.lock") + 1 = 15
    strncpy(path, home, home_path_len + 1);
    strncat(path, "/.", 3);
    strncat(path, name, strlen(name) + 1);
    strncat(path, ".lock", 6);
    return path;
}

const char *get_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot;
}

char *add_extension(const char *filename, const char *extension) {
    int filename_size = strlen(filename);
    int extension_size = strlen(extension);
    char *new_filename = malloc(sizeof(char)*(filename_size + extension_size + 1));
    if (new_filename == NULL) fatal(strerror(errno));
    strncpy(new_filename, filename, filename_size + 1);
    strncat(new_filename, extension, extension_size + 1);
    return new_filename;
}

#endif