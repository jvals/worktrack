#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <logger.h>
#include <stdlib.h>

#include "server.h"
#include "request.h"
#include "response.h"
#include "router.h"
#include "error_codes.h"
#include "config_t.h"

#define BACKLOG 4

#ifdef __linux__
#define NOSIGNAL MSG_NOSIGNAL
#elif __APPLE__
#define NOSIGNAL SO_NOSIGPIPE
#endif

int server_accept(server_t* server, config_t* config) {
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
    return MALFORMED_REQUEST;
  } else if (valread < 0) {
    LOGGER(ERROR, "read %s", strerror(errno));
  } else {
    LOGGER(INFO, "Received content:\n%s\n", buffer);
  }

  // Parse request
  request_headers_t headers = {0};
  request_t request = {0};

  // Parse start line
  char* request_raw = strdup(buffer);
  char* original_request_raw = request_raw;
  char* start_line = strsep(&request_raw, "\n");
  if (start_line == NULL) {
    LOGGER(DEBUG, "Start line was NULL\n", "");
    return MALFORMED_REQUEST;
  }

  LOGGER(TRACE, "Parsing start_line: %s\n", start_line);
  char* token = NULL;

  int start_line_idx = 0;
  while ( (token = strsep(&start_line, " ")) != NULL ) {
    if (start_line_idx == 0) {
      LOGGER(TRACE, "token: %s\n", token);
      request.method = strdup(token);
    } else if (start_line_idx == 1) {
      LOGGER(TRACE, "token: %s\n", token);
      request.path = strdup(token);
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
      char* header_value_orig = header_value;
      // Remove first char if blank
      if (header_value[0] == ' ') header_value++;
      if (header_key != NULL && header_value != NULL) {
        if (strcmp(header_key, "Accept-Encoding") == 0) {
          headers.accept_encoding = header_value;
        } else if (strcmp(header_key, "Connection") == 0) {
          headers.connection = header_value;
        } else if (strcmp(header_key, "Host") == 0) {
          headers.host = header_value;
        } else if (strcmp(header_key, "User-Agent") == 0) {
          headers.user_agent = header_value;
        } else if (strcmp(header_key, "Authorization") == 0) {
          headers.authorization = header_value;
        }
        LOGGER(TRACE, "Found header with key=%s and value=%s\n", header_key, header_value);
        free(header_value_orig);
      } else {
        LOGGER(DEBUG, "Malformed headers, dropping request", "");
        return MALFORMED_REQUEST;
      }
    }

    // Two (three empty lines from strsep) newlines indicate that the header section is ending
    if (consecutive_empty == 3) {
      break;
    }
  }
  free(original_request_raw);

  request.headers = headers;

  // TODO Parse body

  // Verify request struct
  if (request.path == NULL) {
    LOGGER(DEBUG, "No path found in request, dropping request\n", "");
    return MALFORMED_REQUEST;
  }

  if (request.method == NULL) {
    LOGGER(DEBUG, "No method found in request, dropping request\n", "");
    return MALFORMED_REQUEST;
  }

  // Route the request to matching controller and action
  response_t response = route(request, config);

  // free request members
  free(request.method);
  free(request.path);

  // Compute content-length of body
  if (response.body != NULL) {
    size_t content_length = strlen(response.body);
    response.content_length = content_length;
  }

  LOGGER(TRACE, "Response body: %s\n", response.body);

  char response_raw[2048];
  // TODO: Optional headers
  sprintf(response_raw, "HTTP/1.1 %d %s\nContent-Type: %s\nContent-Length: %d\nLocation: %s\nAccess-Control-Allow-Origin: *\n\n%s",
          response.status_code,
          response.status_message,
          response.content_type,
          response.content_length,
          response.location,
          response.body == NULL ? "" : response.body);

  // response body is populated by strdup, which uses malloc
  if (response.body != NULL) {
    free(response.body);
  }

  err = send(conn_fd, response_raw, strlen(response_raw), NOSIGNAL);
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
    LOGGER(FATAL, "socket %s\n", strerror(errno));
    LOGGER(FATAL, "Failed to create socket endpoint\n", "");
    return err;
  }

  // Allow server to reuse socket even if TIME_WAIT seconds has not passed
  // Useful when developing and iterating fast; Fatal errors will often result in sockets remaining open and thus not allowing themselves to be reused for a set amount of time.
  // Explanation: https://stackoverflow.com/a/3233022
  // int option = 1;
  // setsockopt(server->listen_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  err = bind(server->listen_fd, (struct sockaddr*)&server_addr,
             sizeof(server_addr));
  if (err == -1) {
    LOGGER(FATAL, "bind %s\n", strerror(errno));
    LOGGER(FATAL, "Failed to bind socket to address\n", "");
    return err;
  }

  err = listen(server->listen_fd, BACKLOG);
  if (err == -1) {
    LOGGER(FATAL, "listen %s\n", strerror(errno));
    LOGGER(FATAL, "Failed to put socket in passive mode\n", "");
    return err;
  }

  return err;
}
