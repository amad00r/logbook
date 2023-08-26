#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>
#include <dirent.h>



void usage(void) {
    puts(
        "Usage: ./logbook [entry|......]\n"
        "logbook is a simple command-line utility for safely storing your thoughts.\n"
        "### Amadeu Moya Sardà"
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


int main(int argc, char **argv) {

    assert(0 && "TODO: list and list_locations");

    if (argc == 1 || (argc == 2 && !strcmp(argv[1], "help"))) usage();

    else if (!strcmp(argv[1], "list")) {
        if (argc != 2) fatal("unexpected parameters, use `logbook help`");

        unsigned int logbook_counter = 0;

        const char *home = getenv("HOME");
        DIR *dir = opendir(home);
        if (dir == NULL) fatal(strerror(errno));
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
            if (entry->d_type == DT_REG && !strcmp(get_extension(entry->d_name), ".lock")) {
                fputs(home, stdout);
                putchar('/');
                puts(entry->d_name);
                ++logbook_counter;
            }

        if (logbook_counter == 0) info("did not found any logbook, to create one use `logbook create <LOGBOOK_NAME>`");
        else if (logbook_counter == 1) info("found 1 logbook");
        else info("found %u logbooks", logbook_counter);
    }

    else if (!strcmp(argv[1], "create")) {
        if (argc != 3) fatal("unexpected parameters, use `logbook help`");

        char *filepath = get_logbook_path(argv[2]);
        if (!access(filepath, F_OK)) fatal("`%s` already exists", filepath);
        int fd = open(filepath, O_CREAT | O_WRONLY);
        if (fd == -1 || close(fd) == -1) fatal(strerror(errno));
        info("`%s` successfully created", filepath);

        free(filepath);
    }

    else if (!strcmp(argv[1], "delete")) {
        if (argc != 3) fatal("unexpected parameters, use `logbook help`");

        char *filepath = get_logbook_path(argv[2]);
        if (access(filepath, F_OK)) fatal("`%s` does not exist", filepath);
        if (remove(filepath)) fatal(strerror(errno));
        info("`%s` successfully removed", filepath);

        free(filepath);
    }

    else fatal("unexpected parameters, use `logbook help`");

    return EXIT_SUCCESS;
}