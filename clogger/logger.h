#define LOGGER(LEVEL, FORMAT_STRING, ...)                         \
  logger((LEVEL), __FILE__, __LINE__, FORMAT_STRING, __VA_ARGS__)

#define CODE_RED "[0;31m"
#define CODE_BOLD_RED "[1;31m"
#define CODE_YELLOW "[0;33m"
#define CODE_MAGENTA "[0;35m"
#define CODE_RESET "[0m"

enum LOG_LEVELS { ALL, TRACE, DEBUG, INFO, WARN, ERROR, FATAL, OFF };
#ifdef LOG_ALL
#define LOG_LIMIT ALL
#endif
#ifdef LOG_TRACE
#define LOG_LIMIT TRACE
#endif
#ifdef LOG_DEBUG
#define LOG_LIMIT DEBUG
#endif
#ifdef LOG_INFO
#define LOG_LIMIT INFO
#endif
#ifdef LOG_WARN
#define LOG_LIMIT WARN
#endif
#ifdef LOG_ERROR
#define LOG_LIMIT ERROR
#endif
#ifdef LOG_FATAL
#define LOG_LIMIT FATAL
#endif
#ifdef LOG_OFF
#define LOG_LIMIT OFF
#endif

// Default level
#ifndef LOG_LIMIT
#define LOG_LIMIT ERROR
#endif

void print_colored_time_and_date(const char* color_code, const char* status,
                                 const char* file_name, int line_number);

void reset_color(void);

void logger(enum LOG_LEVELS log_level, const char* file_name, int line_number,
            const char* string, ...);
