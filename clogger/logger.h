#define LOGGER(LEVEL, FORMAT_STRING, ...)                         \
  logger((LEVEL), __FILE__, __LINE__, FORMAT_STRING, __VA_ARGS__)

#define CODE_RED "[0;31m"
#define CODE_YELLOW "[0;33m"
#define CODE_RESET "[0m"

enum LOG_LEVELS { INFO, WARN, ERROR };
#ifdef LOG_INFO
#define LOG_LIMIT INFO
#endif
#ifdef LOG_WARN
#define LOG_LIMIT WARN
#endif
#ifdef LOG_ERROR
#define LOG_LIMIT ERROR
#endif
#ifndef LOG_LIMIT
#define LOG_LIMIT ERROR
#endif

void print_colored_time_and_date(const char* color_code, const char* status,
                                 const char* file_name, int line_number);

void reset_color(void);

void logger(enum LOG_LEVELS log_level, const char* file_name, int line_number,
            const char* string, ...);
