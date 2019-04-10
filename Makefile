CC := gcc
LD := gcc

INCLUDES := -Iclogger
LIBS := -Lclogger

CFLAGS := -O3 -Wall -Wextra -Wpedantic -march=native -std=gnu11 -D LOG_INFO $(INCLUDES)
LDFLAGS := $(LIBS) -llogger -lsqlite3
TARGET := server

SRCDIR := src
OBJDIR := obj

SRC_FILES := $(wildcard $(SRCDIR)/*.c)
OBJ_FILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC_FILES))

$(TARGET): $(OBJ_FILES)
	$(LD) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean purge run

clean:
	rm -rf $(TARGET)

purge: clean
	rm -rf $(OBJDIR)

run: $(TARGET)
	./$<
