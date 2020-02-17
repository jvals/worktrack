#include <logger.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "time_controller.h"
#include "time_service.h"

#define UNUSED(x) (void)(x)

response_t get_total_time(request_t req) {
  UNUSED(req);
  response_t response = {0};

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
  UNUSED(req);
  response_t response = {0};

  time_service_create_entry();

  response.status_code = 201;
  response.status_message = "Created";
  response.content_length = 0;
  response.content_type = "";
  response.body = NULL;

  return response;
}

response_t stop_time(request_t req) {
  UNUSED(req);
  response_t response = {0};

  time_service_stop_latest_entry();

  response.status_code = 204;
  response.status_message = "No Content";
  response.content_length = 0;
  response.content_type = "";
  response.body = NULL;

  return response;
}

response_t get_todays_time(request_t req) {
  UNUSED(req);
  response_t response = {0};

  uint64_t todays_diff = 0;
  todays_diff = time_service_get_todays_diff();
  char http_body[128];
  sprintf(http_body, "%lld", todays_diff);

  response.status_code = 200;
  response.status_message = "OK";
  response.content_length = 1024;
  response.content_type = "text/plain";
  response.body = strdup(http_body);

  return response;
}

response_t get_overtime(request_t req) {
  UNUSED(req);
  response_t response = {0};

  uint64_t overtime = 0;
  overtime = time_service_get_overtime_with_compensation();
  char http_body[128];
  sprintf(http_body, "%lld", overtime);

  response.status_code = 200;
  response.status_message = "OK";
  response.content_length = 1024;
  response.content_type = "text/plain";
  response.body = strdup(http_body);

  return response;
}

response_t get_work_in_progress(request_t req) {
  UNUSED(req);
  response_t response = {0};

  bool work_in_progress = false;
  work_in_progress = time_service_unfinished_work();
  char http_body[128];
  sprintf(http_body, "%s", work_in_progress ? "true" : "false");

  response.status_code = 200;
  response.status_message = "OK";
  response.content_length = 1024;
  response.content_type = "text/plain";
  response.body = strdup(http_body);

  return response;
}

response_t get_all_work(request_t req) {
  UNUSED(req);
  response_t response = {0};

  char* all_work = NULL;
  time_service_get_all_work(&all_work);

  response.status_code = 200;
  response.status_message = "OK";
  response.content_length = 1024;
  response.content_type = "text/plain";
  response.body = strdup((all_work == NULL) ? "" : all_work);

  if (all_work != NULL) {
    free(all_work);
  }

  return response;
}

// TODO move this to somewhere else
static bool stringIsNumeric(char* str) {
  char *character;
  for (character = str; *character != '\0'; character++) {
    if (!isdigit(*character)) {
      return false;
    }
  }
  return true;
}

response_t update_work_start(request_t req) {
  response_t response = {0};

  char *id = strsep(&(req.body), ";");
  char *new_work_start = req.body;

  if (stringIsNumeric(id) && stringIsNumeric(new_work_start)) {
    time_service_update_work_start(id, new_work_start);
  }

  response.status_code = 204;
  response.status_message = "No Content";
  response.content_length = 0;
  response.content_type = "";
  response.body = NULL;

  return response;
}

response_t update_work_end(request_t req) {
  response_t response = {0};

  char *id = strsep(&(req.body), ";");
  char *new_work_start = req.body;

  if (stringIsNumeric(id) && stringIsNumeric(new_work_start)) {
    time_service_update_work_end(id, new_work_start);
  }

  response.status_code = 204;
  response.status_message = "No Content";
  response.content_length = 0;
  response.content_type = "";
  response.body = NULL;

  return response;
}

response_t update_work_start_options(request_t req) {
  response_t response = {0};
  response.status_code = 204;
  response.status_message = "No Content";
  response.content_length = 0;
  response.content_type = "";
  response.allow = "OPTIONS, PATCH";
  response.body = NULL;

  return response;
}

response_t update_work_end_options(request_t req) {
  response_t response = {0};
  response.status_code = 204;
  response.status_message = "No Content";
  response.content_length = 0;
  response.content_type = "";
  response.allow = "OPTIONS, PATCH";
  response.body = NULL;

  return response;
}
