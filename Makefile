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
	@mkdir -pv $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean purge run

clean:
	rm -rfv $(TARGET)

purge: clean
	rm -rfv $(OBJDIR)

run: $(TARGET)
	./$<


# CMOCKA
MOCKS=safe_new_entry # list of function signature to mock
TEST_LDFLAGS+=$(foreach MOCK,$(MOCKS),-Wl,--wrap=$(MOCK))
TEST_LDFLAGS+=-lcmocka

TESTDIR := test
TEST_SRC_FILES := $(wildcard $(TESTDIR)/*.c)
TEST_OBJ_FILES := $(patsubst $(TESTDIR)/%.c, $(OBJDIR)/%.o, $(TEST_SRC_FILES))

TEST_TARGET = test_runner

$(TEST_TARGET): $(TEST_OBJ_FILES) $(OBJ_FILES)
	$(LD) -o $@ $^ $(LDFLAGS) $(TEST_LDFLAGS)

$(OBJDIR)/%.o: $(TESTDIR)/%.c
	@mkdir -pv $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<
