#include <logger.h>

#include "time_controller.h"

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
