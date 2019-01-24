#pragma once

typedef struct headers {
  char* accept_encoding;
  char* connection;
  char* host;
  char* user_agent;
} request_headers_t;

typedef struct request {
  char* path;
  char* method;
  request_headers_t headers;
  char* body;
} request_t;
