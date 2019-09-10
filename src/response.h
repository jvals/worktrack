#pragma once

typedef struct response {
  int status_code;
  char* status_message;
  int content_length;
  char* content_type;
  char* location;
  char* body;
} response_t;
