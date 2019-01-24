#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <logger.h>
#include <stdlib.h>

#include "server.h"

#define BACKLOG 4

#ifdef __linux__
#define NOSIGNAL MSG_NOSIGNAL
#elif __APPLE__
#define NOSIGNAL SO_NOSIGPIPE
#endif

typedef struct headers {
  char* accept_encoding;
  char* connection;
  char* host;
  char* user_agent;
} headers_t;

typedef struct request {
  char* path;
  char* method;
  headers_t headers;
  char* body;
} request_t;

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

  // Parse request
  headers_t headers = {0};
  request_t request = {0};

  // Parse start line
  char* request_raw = strdup(buffer);
  char* original_request_raw = request_raw;
  char* start_line = strsep(&request_raw, "\n");
  if (start_line == NULL) {
    LOGGER(DEBUG, "Start line was NULL\n", "");
    // TODO: Error handling
  }

  LOGGER(TRACE, "Parsing start_line: %s\n", start_line);
  char* token = NULL;

  int start_line_idx = 0;
  while ( (token = strsep(&start_line, " ")) != NULL ) {
    if (start_line_idx == 0) {
      request.method = token;
    } else if (start_line_idx == 1) {
      request.path = token;
    }
    start_line_idx++;
  }

  // Parse headers
  LOGGER(TRACE, "Parsing headers\n", "");
  char* request_line = NULL;
  int consecutive_empty = 0; // Detecting when headers end and body begin
  while ( (request_line = strsep(&request_raw, "\r\n")) != NULL ) {
    if (strcmp(request_line, "") == 0) {
      consecutive_empty++;
    } else {
      consecutive_empty = 0;
      // Headers are of the format key:value, so we need to split again
      char* header_key = strsep(&request_line, ":");
      char* header_value = strdup(request_line);
      if (header_key != NULL && header_value != NULL) {
        if (strcmp(header_key, "Accept-Encoding")) {
          headers.accept_encoding = header_value;
        } else if (strcmp(header_key, "Connection")) {
          headers.connection = header_value;
        } else if (strcmp(header_key, "Host")) {
          headers.host = header_value;
        } else if (strcmp(header_key, "User-Agent")) {
          headers.user_agent = header_value;
        }
        LOGGER(TRACE, "Found header with key=%s and value=%s\n", header_key, header_value);
        free(header_value);
      } else {
        LOGGER(DEBUG, "Malformed headers, dropping request", "");
        // TODO: Error handling
      }
    }

    // Two (three empty lines from strsep) newlines indicate that the header section is ending
    if (consecutive_empty == 3) {
      break;
    }
  }
  free(original_request_raw);

  request.headers = headers;

  // Parse body
  LOGGER(TRACE, "Parsing request body\n", "");
  LOGGER(DEBUG, "Ignoring request body\n", "");

  // Verify request struct
  if (request.path == NULL) {
    LOGGER(DEBUG, "No path found in request, dropping request\n", "");
    // TODO: Error handling
  }

  if (request.method == NULL) {
    LOGGER(DEBUG, "No method found in request, dropping request\n", "");
    // TODO: Error handling
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
