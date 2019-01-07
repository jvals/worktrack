#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <logger.h>

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
  err = (conn_fd = accept(server->listen_fd, (struct sockaddr*)&client_addr,
                          &client_len));
  if (err == -1) {
    LOGGER(ERROR, "accept %s", strerror(errno));
    printf("failed accepting connection\n");
    return err;
  }

  printf("Client connected!\n");

  char buffer[1024] = {0};
  int valread = read(conn_fd, buffer, 1024);
  printf("%s\n", buffer);
  if (valread == 0) {
    printf("No bytes to read\n");
  } else if (valread < 0) {
    LOGGER(ERROR, "read %s", strerror(errno));
  }

  char* response =
      "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello "
      "World!";
  err = send(conn_fd, response, strlen(response), SO_NOSIGPIPE);
  if (err == -1) {
      LOGGER(ERROR, "send %s", strerror(errno));
  }
  err = close(conn_fd);
  if (err == -1) {
    LOGGER(ERROR, "close %s", strerror(errno));
    printf("failed to close connection\n");
    return err;
  }

  return err;
}

int server_listen(server_t* server) {
  int err = 0;
  struct sockaddr_in server_addr = {0};

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

  err = (server->listen_fd = socket(AF_INET, SOCK_STREAM, 0));
  if (err == -1) {
    LOGGER(ERROR, "socket %s", strerror(errno));
    printf("Failed to create socket endpoint\n");
    return err;
  }

  err = bind(server->listen_fd, (struct sockaddr*)&server_addr,
             sizeof(server_addr));
  if (err == -1) {
    LOGGER(ERROR, "bind %s", strerror(errno));
    printf("Failed to bind socket to address\n");
    return err;
  }

  err = listen(server->listen_fd, BACKLOG);
  if (err == -1) {
    LOGGER(ERROR, "listen %s", strerror(errno));
    printf("Failed to put socket in passive mode\n");
    return err;
  }

  return err;
}

int main() {
  int err = 0;
  server_t server = {0};

  err = server_listen(&server);
  if (err) {
    printf("Failed to listen on address 0.0.0.0:%d\n", PORT);
    return err;
  }

  while (1) {
    err = server_accept(&server);
    if (err) {
      printf("Failed accepting connection\n");
      return err;
    }
  }

  return 0;
}
