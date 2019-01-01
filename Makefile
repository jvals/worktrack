CC := gcc
LD := gcc

CFLAGS := -O3 -Wall -Wextra -Wpedantic -march=native -std=c11 -D LOG_INFO

TARGET := server

SRCDIR := src
OBJDIR := obj

SRC_FILES := $(wildcard $(SRCDIR)/*.c)
OBJ_FILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC_FILES))



$(TARGET): $(OBJ_FILES)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean purge

clean:
	rm -rf $(TARGET)

purge: clean
	rm -rf $(OBJDIR)
