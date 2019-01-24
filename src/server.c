#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <logger.h>

#include "server.h"

#define BACKLOG 4

#ifdef __linux__
#define NOSIGNAL MSG_NOSIGNAL
#elif __APPLE__
#define NOSIGNAL SO_NOSIGPIPE
#endif

int server_accept(server_t* server) {
  int err = 0;
  int conn_fd;
  socklen_t client_len;
  struct sockaddr_in client_addr;

  client_len = sizeof(client_addr);
  err = (conn_fd = accept(server->listen_fd, (struct sockaddr*)&client_addr,
                          &client_len));
  if (err == -1) {
    LOGGER(FATAL, "accept %s", strerror(errno));
    LOGGER(FATAL, "failed accepting connection\n", "");
    return err;
  }

  LOGGER(TRACE, "Client connected!\n", "");

  char buffer[1024] = {0};
  int valread = read(conn_fd, buffer, 1024);
  if (valread == 0) {
    LOGGER(DEBUG, "No bytes to read\n", "");
  } else if (valread < 0) {
    LOGGER(ERROR, "read %s", strerror(errno));
  } else {
    LOGGER(INFO, "Received content:\n%s\n", buffer);
  }

  char* response =
      "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello "
      "World!";
  err = send(conn_fd, response, strlen(response), NOSIGNAL);
  if (err == -1) {
      LOGGER(ERROR, "send %s", strerror(errno));
  }
  err = close(conn_fd);
  if (err == -1) {
    LOGGER(FATAL, "close %s", strerror(errno));
    LOGGER(FATAL, "failed to close connection\n", "");
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
    LOGGER(FATAL, "socket %s", strerror(errno));
    LOGGER(FATAL, "Failed to create socket endpoint\n", "");
    return err;
  }

  err = bind(server->listen_fd, (struct sockaddr*)&server_addr,
             sizeof(server_addr));
  if (err == -1) {
    LOGGER(FATAL, "bind %s", strerror(errno));
    LOGGER(FATAL, "Failed to bind socket to address\n", "");
    return err;
  }

  err = listen(server->listen_fd, BACKLOG);
  if (err == -1) {
    LOGGER(FATAL, "listen %s", strerror(errno));
    LOGGER(FATAL, "Failed to put socket in passive mode\n", "");
    return err;
  }

  return err;
}
