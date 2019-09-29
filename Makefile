CC ?= gcc
LD ?= gcc

INCLUDES := -Iclogger
LIBS := -Lclogger

CFLAGS += -O3 -Wall -Wextra -Wpedantic -march=native -std=gnu11 -D LOG_INFO $(INCLUDES)
LDFLAGS += $(LIBS) -llogger -lsqlite3
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

.PHONY: clean purge run test

clean:
	rm -rfv $(TARGET)

purge: clean
	rm -rfv $(OBJDIR)

run: $(TARGET)
	./$<


# Test specific rules
TEST_BINARIES = test_time_service_runner test_db_utils_runner
EXEC_TEST_BINARIES = $(foreach TEST_BINARY, $(TEST_BINARIES), ./$(TEST_BINARY); )
test: $(TEST_BINARIES)
	$(EXEC_TEST_BINARIES)

MOCKS=safe_new_entry time patch_latest get_total_diff logger sqlite3_open # list of function signature to mock
TEST_LDFLAGS+=$(foreach MOCK,$(MOCKS),-Wl,--wrap=$(MOCK))
TEST_LDFLAGS+=-lcmocka

test_time_service_runner: $(OBJDIR)/test_time_service.o $(OBJDIR)/time_service.o
	$(LD) -o $@ $^ $(TEST_LDFLAGS) $(LDFLAGS)

TESTDIR := test
TEST_SRC_FILES := $(wildcard $(TESTDIR)/*.c)
TEST_OBJ_FILES := $(patsubst $(TESTDIR)/%.c, $(OBJDIR)/%.o, $(TEST_SRC_FILES))

# TEST_TARGET = test_runner

# $(TEST_TARGET): $(TEST_OBJ_FILES) $(OBJ_FILES)
# 	$(LD) -o $@ $^ $(LDFLAGS) $(TEST_LDFLAGS)

$(OBJDIR)/%.o: $(TESTDIR)/%.c
	@mkdir -pv $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<
