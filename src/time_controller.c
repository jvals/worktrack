#include <logger.h>

#include "time_controller.h"

response_t get_total_time(request_t req) {
  response_t response;
  LOGGER(INFO, "Getting the total time\n", "");

  response.status_code = 200;
  response.status_message = "OK";
  response.content_length = 0;
  response.content_type = "";
  response.body = "";

  return response;
}

response_t start_time(request_t req) {
  response_t response;
  LOGGER(INFO, "Creating new time entry\n", "");

  response.status_code = 201;
  response.status_message = "Created";
  response.content_length = 0;
  response.content_type = "";
  response.body = "";

  return response;
}

response_t stop_time(request_t req) {
  response_t response;
  LOGGER(INFO, "Setting stop time on most recent time entry\n", "");

  response.status_code = 204;
  response.status_message = "No Content";
  response.content_length = 0;
  response.content_type = "";
  response.body = "";

  return response;
}
