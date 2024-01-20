#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <dirent.h>
#include <time.h>

#include "log.h"
#include "path.h"
#include "strip.h"


int main(int argc, char **argv) {

    if (argc == 1) quote();

    else if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) usage();

    else if (!strcmp(argv[1], "list")) {
        if (argc != 2) fatal("unexpected parameters, use `logbook -h`");

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
        if (argc != 3) fatal("unexpected parameters, use `logbook -h`");

        char *filepath = get_logbook_path(argv[2]);
        if (!access(filepath, F_OK)) fatal("`%s` already exists", filepath);
        int fd = open(filepath, O_CREAT | O_WRONLY);
        if (fd == -1 || close(fd) == -1) fatal(strerror(errno));
        info("`%s` successfully created", filepath);

        free(filepath);
    }

    else if (!strcmp(argv[1], "delete")) {
        if (argc != 3) fatal("unexpected parameters, use `logbook -h`");

        char *filepath = get_logbook_path(argv[2]);
        if (remove(filepath)) fatal("could not remove `%s`", filepath);
        info("`%s` successfully removed", filepath);

        free(filepath);
    }

    else if (!strcmp(argv[1], "write")) {
        if (argc != 3) fatal("unexpected parameters, use `logbook -h`");

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

    else if (!strcmp(argv[1], "gen_pdf")) {
        if (argc != 3 && argc != 4) fatal("unexpected parameters, use `logbook -h`");

        char *filepath = get_logbook_path(argv[2]);
        if (access(filepath, F_OK)) fatal("`%s` does not exist", filepath);

        int fd = open(filepath, O_RDONLY);
        if (fd == -1) fatal(strerror(errno));

        free(filepath);

        off_t size = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        char *logbook_buf = malloc(sizeof(char)*(size + 1));
        if (logbook_buf == NULL || read(fd, logbook_buf, size) == -1) fatal(strerror(errno));
        logbook_buf[size] = EOF;

        char *md_path = add_extension(argv[2], ".md");

        FILE *md = fopen(md_path, "w");
        if (md == NULL) fatal(strerror(errno));

        char *field = logbook_buf;
        char c;
        while (*field != EOF) {
            fputs("## ", md);
            for (int i = 0; (c = field[i]) != '\0'; ++i) {
                if (c == '\n') fputs("\n\n", md);
                else fputc(c, md);
            }
            fputs("\n\n", md);
            field += strlen(field) + 1;

            fputs("### ", md);
            for (int i = 0; (c = field[i]) != '\0'; ++i) {
                if (c == '\n') fputs("\n\n", md);
                else fputc(c, md);
            }
            fputs("\n\n", md);
            field += strlen(field) + 1;

            for (int i = 0; (c = field[i]) != '\0'; ++i) {
                if (c == '\n') fputs("\n\n", md);
                else fputc(c, md);
            }
            fputs("\n\n", md);
            field += strlen(field) + 1;
        }

        free(logbook_buf);

        if (fclose(md) == -1) fatal(strerror(errno));

        
        int md_path_size = strlen(md_path);
        char *pdf_path;
        if (argc == 4) pdf_path = argv[3];
        else           pdf_path = add_extension(argv[2], ".pdf");
        int pdf_path_size = strlen(pdf_path);
        char *command = malloc(sizeof(char)*(md_path_size + pdf_path_size + 12));
        if (command == NULL) fatal(strerror(errno));
        strncpy(command, "pandoc ", 8);
        strncat(command, md_path, md_path_size + 1);
        strncat(command, " -o ", 5);
        strncat(command, pdf_path, pdf_path_size + 1);
        if (argc == 3) free(pdf_path);

        system(command);
        free(command);

        if (remove(md_path) == -1) fatal(strerror(errno));
        free(md_path);
    }

    else fatal("unexpected parameters, use `logbook -h`");

    return EXIT_SUCCESS;
}
