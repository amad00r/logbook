#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <dirent.h>
#include <time.h>

#include "log.h"


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


char *strip(char *str) {
    char *stripped_str = str;
    char c = *stripped_str;
    while (c == ' ' || c == '\n' || c == '\t') c = *++stripped_str;
    unsigned int size = strlen(stripped_str);
    while (size > 0 && ((c = stripped_str[size - 1]) == ' ' || c == '\n' || c == '\t' || c == EOF)) --size;
    if (size > 0) stripped_str[size] = '\0';
    return stripped_str;
}


int main(int argc, char **argv) {

    if (argc == 1) quote();

    else if (argc == 2 && !strcmp(argv[1], "help")) usage();

    else if (!strcmp(argv[1], "list")) {
        if (argc != 2) fatal("unexpected parameters, use `logbook help`");

        unsigned int logbook_counter = 0;

        const char *home = getenv("HOME");
        DIR *dir = opendir(home);
        if (dir == NULL) fatal(strerror(errno));
        struct dirent *entry;
        errno = 0;
        while ((entry = readdir(dir)) != NULL)
            if (entry->d_type == DT_REG && !strcmp(get_extension(entry->d_name), ".lock")) {
                fputs(home, stdout);
                putchar('/');
                puts(entry->d_name);
                ++logbook_counter;
            }
        if (errno || closedir(dir) == -1) fatal(strerror(errno));

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
        if (remove(filepath)) fatal("could not remove `%s`", filepath);
        info("`%s` successfully removed", filepath);

        free(filepath);
    }

    else if (!strcmp(argv[1], "write")) {
        if (argc != 3) fatal("unexpected parameters, use `logbook help`");

        char *filepath = get_logbook_path(argv[2]);
        if (access(filepath, F_OK)) fatal("`%s` does not exist", filepath);

        FILE *f = fopen(filepath, "a");
        if (f == NULL) fatal("`%s` could not be opened", filepath);
        
        fputs("(title): ", stdout);
        size_t size = 0;
        char *input1 = NULL;
        if (getline(&input1, &size, stdin) == -1) fatal(strerror(errno));
        
        puts("(body):\n");
        size = 0;
        char *input2 = NULL;
        if (getdelim(&input2, &size, EOF, stdin) == -1) fatal(strerror(errno));

        time_t t = time(NULL);
        char *current_time = ctime(&t);
        current_time[strlen(current_time) - 1] = '\0';

        char *title = strip(input1);
        char *body = strip(input2);

        fputs(current_time, f);
        putc('\0', f);
        fputs(title, f);
        putc('\0', f);
        fputs(body, f);
        putc('\0', f);

        putchar('\n');
        info("%u bytes message successfully logged", strlen(body));

        if (fclose(f) == -1) fatal(strerror(errno));

        free(filepath);
    }

    else fatal("unexpected parameters, use `logbook help`");

    return EXIT_SUCCESS;
}
