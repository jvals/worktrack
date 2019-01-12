CFLAGS := -O3 -Wall -Wextra -Wpedantic -march=native -D LOG_ALL

liblogger.a: logger.o
	ar rcs $@ $<

.PHONY: clean purge

clean:
	rm -rf *.o

purge: clean
	rm -rf *.a
