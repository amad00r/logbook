CC = clang
DEPS = src/log.h src/path.h src/strip.h
OBJ = bin/logbook.o

bin/%.o: src/%.c $(DEPS)
	$(CC) -Wall -Wextra -ggdb -c -o $@ $<

bin/logbook: $(OBJ)
	$(CC) -Wall -Wextra -ggdb -o $@ $^

clear:
	rm bin/*