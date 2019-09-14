#include <logger.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "dashboard_controller.h"
#include "time_service.h"

#define UNUSED(x) (void)(x)

char *replaceWord(const char *s, const char *oldW,
                                 const char *newW)
{
    char *result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(oldW);

    // Counting the number of times old word
    // occur in the string
    for (i = 0; s[i] != '\0'; i++)
    {
        if (strstr(&s[i], oldW) == &s[i])
        {
            cnt++;

            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }

    // Making new string of enough length
    result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);

    i = 0;
    while (*s)
    {
        // compare the substring with the result
        if (strstr(s, oldW) == s)
        {
            strcpy(&result[i], newW);
            i += newWlen;
            s += oldWlen;
        }
        else
            result[i++] = *s++;
    }

    result[i] = '\0';
    return result;
}

response_t get_dashboard(request_t req) {
  UNUSED(req);
  response_t response;

  char http_body[1024] = {0};

  FILE *index = fopen("index.html", "rb");
  fread(http_body, 1024, 1, index);
  fclose(index);

  bool status = time_service_unfinished_work();

  char *http_body_status = NULL;
  char *http_body_button = NULL;

  char status_text[8] = {0};
  char button_text[8] = {0};
  if (status) {
    sprintf(status_text, "Working");
    sprintf(button_text, "Stop");
  } else {
    sprintf(status_text, "Idle");
    sprintf(button_text, "Start");
  }

  http_body_status = replaceWord(http_body, "{{ status }}", status_text);
  http_body_button = replaceWord(http_body_status, "{{ buttonText }}", button_text);


  response.status_code = 200;
  response.status_message = "OK";
  response.content_length = 1024;
  response.content_type = "text/html";
  response.body = strdup(http_body_button);

  free(http_body_status);
  free(http_body_button);

  return response;
}

response_t post_dashboard(request_t req) {
  UNUSED(req);
  response_t response;

  // Toggle time
  bool status = time_service_unfinished_work();
  if (status) {
    time_service_stop_latest_entry();
  } else {
    time_service_create_entry();
  }

  response.status_code = 303;
  response.status_message = "See Other";
  response.content_length = 0;
  response.content_type = "";
  response.location = "/dashboard";
  response.body = "";

  return response;
}
