#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include "logger.h"


void print_colored_time_and_date(const char* color_code, const char* status,
                                 const char* file_name, int line_number) {
  printf("\033%s", color_code);
  printf("[%s ", status);
  time_t now;
  time(&now);
  char time_string[32];
  strftime(time_string, sizeof(time_string), "%FT%T%z", localtime(&now));
  printf("%s ", time_string);
  printf("F:%s ", file_name);
  printf("L:%d] ", line_number);
}

void reset_color() { printf("\033%s\n", CODE_RESET); }

void logger(enum LOG_LEVELS log_level, const char* file_name, int line_number,
            const char* string, ...) {
  va_list arg;
  va_start(arg, string);
  if (log_level == FATAL && LOG_LIMIT <= FATAL) {
    print_colored_time_and_date(CODE_BOLD_RED, "FATAL", file_name, line_number);
    vfprintf(stdout, string, arg);
    reset_color();
  } else if (log_level == ERROR && LOG_LIMIT <= ERROR) {
    print_colored_time_and_date(CODE_RED, "ERROR", file_name, line_number);
    vfprintf(stdout, string, arg);
    reset_color();
  } else if (log_level == WARN && LOG_LIMIT <= WARN) {
    print_colored_time_and_date(CODE_YELLOW, "WARN", file_name, line_number);
    vfprintf(stdout, string, arg);
    reset_color();
  } else if (log_level == INFO && LOG_LIMIT <= INFO) {
    print_colored_time_and_date(CODE_RESET, "INFO", file_name, line_number);
    vfprintf(stdout, string, arg);
    printf("\n");
  } else if (log_level == DEBUG && LOG_LIMIT <= DEBUG) {
    print_colored_time_and_date(CODE_MAGENTA, "DEBUG", file_name, line_number);
    vfprintf(stdout, string, arg);
    reset_color();
  } else if (log_level == TRACE && LOG_LIMIT <= TRACE) {
    print_colored_time_and_date(CODE_RESET, "TRACE", file_name, line_number);
    vfprintf(stdout, string, arg);
    reset_color();
  }
  va_end(arg);
}
