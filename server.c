#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define PORT 8080
#define BACKLOG 4

typedef struct server {
  int listen_fd;
} server_t;

int server_accept(server_t* server) {
  int err = 0;
  int conn_fd;
  socklen_t client_len;
  struct sockaddr_in client_addr;

  client_len = sizeof(client_addr);
  err = (conn_fd = accept(server->listen_fd, (struct sockaddr*)&client_addr, &client_len));
  if (err == -1) {
    perror("accept");
    printf("failed accepting connection\n");
    return err;
  }

  printf("Client connected!\n");

  char buffer[1024] = {0};
  int valread = read(conn_fd, buffer, 1024);
  printf("%s\n", buffer);
  if (valread < 0) {
    printf("No bytes to read\n");
  }

  char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello World!";
  write(conn_fd, response, strlen(response));

  err = close(conn_fd);
  if (err == -1) {
    perror("close");
    printf("failed to close connection\n");
    return err;
  }

  return err;
}

int server_listen(server_t* server) {
  int err = 0;
  struct sockaddr_in server_addr = { 0 };

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

  err = (server->listen_fd = socket(AF_INET, SOCK_STREAM, 0));
  if (err == -1) {
    perror("socket");
    printf("Failed to create socket endpoint\n");
    return err;
  }

  err = bind(server->listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (err == -1) {
    perror("bind");
    printf("Failed to bind socket to address\n");
    return err;
  }

  err = listen(server->listen_fd, BACKLOG);
  if (err == -1) {
    perror("listen");
    printf("Failed to put socket in passive mode\n");
    return err;
  }
}
#define LOGGER(LEVEL, FORMAT_STRING, ...) logger((LEVEL), __FILE__, __LINE__, FORMAT_STRING, __VA_ARGS__)


#define CODE_RED "[0;31m"
#define CODE_YELLOW "[0;33m"
#define CODE_RESET "[0m"

enum LOG_LEVELS {INFO, WARN, ERROR};
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

void print_colored_time_and_date(const char* color_code,
                                 const char* status,
                                 const char* file_name,
                                 int line_number) {
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

void reset_color() {
  printf("\033%s\n", CODE_RESET);
}

void logger(enum LOG_LEVELS log_level, const char* file_name, int line_number, const char* string, ...) {
  va_list arg;
  va_start (arg, string);
  if (log_level == ERROR && LOG_LIMIT <= ERROR) {
    print_colored_time_and_date(CODE_RED, "ERROR", file_name, line_number);
    vfprintf (stdout, string, arg);
    reset_color();
  } else if (log_level == WARN && LOG_LIMIT <= WARN) {
    print_colored_time_and_date(CODE_YELLOW, "WARN", file_name, line_number);
    vfprintf (stdout, string, arg);
    reset_color();
  } else if (log_level == INFO && LOG_LIMIT <= INFO) {
    print_colored_time_and_date(CODE_RESET, "INFO", file_name, line_number);
    vfprintf(stdout, string, arg);
    printf("\n");
  }
  va_end (arg);
}

int main()
{
  int err = 0;
  server_t server = { 0 };
  LOGGER(ERROR, "hello you %d %d", 5, 4);
  LOGGER(WARN, "hello you %d %d", 5, 4);
  LOGGER(INFO, "hello you %d %d", 5, 4);
  return 0;

  err = server_listen(&server);
  if(err) {
    printf("Failed to listen on address 0.0.0.0:%d\n", PORT);
    return err;
  }

  while(1) {
    err = server_accept(&server);
    if (err) {
      printf("Failed accepting connection\n");
      return err;
    }
  }

  return 0;
}
