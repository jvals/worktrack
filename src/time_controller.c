#include <logger.h>

#include "time_controller.h"
#include "time_service.h"

response_t get_total_time(request_t req) {
  response_t response;

  uint64_t total_diff = 0;
  total_diff = time_service_get_total_of_diffs();
  char http_body[128];
  sprintf(http_body, "%lld", total_diff);

  response.status_code = 200;
  response.status_message = "OK";
  response.content_length = 1024;
  response.content_type = "text/plain";
  response.body = strdup(http_body);

  return response;
}

response_t start_time(request_t req) {
  response_t response;

  time_service_create_entry();

  response.status_code = 201;
  response.status_message = "Created";
  response.content_length = 0;
  response.content_type = "";
  response.body = "";

  return response;
}

response_t stop_time(request_t req) {
  response_t response;

  time_service_stop_latest_entry();

  response.status_code = 204;
  response.status_message = "No Content";
  response.content_length = 0;
  response.content_type = "";
  response.body = "";

  return response;
}
