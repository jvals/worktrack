#include <logger.h>
#include <stdlib.h>
#include <string.h>

#include "dashboard_controller.h"
#include "route.h"
#include "router.h"
#include "routes_parser.h"
#include "time_controller.h"

typedef struct name_action_mapping {
  char* action_name;
  response_t (*action)(request_t);
} name_action_mapping_t;

name_action_mapping_t* name_action_map;

static void init_name_action_map() {
  int idx = 0;

  // The name_action map is used to name the actions. This allows us
  // to dynamically connect routes to functions. This seems to be the
  // "best" solution without having to create a mini compiler.
  name_action_map = calloc(MAX_ROUTE_ARRAY_SIZE, sizeof(name_action_mapping_t));
  name_action_map[idx].action_name = "get_total_time";
  name_action_map[idx++].action = get_total_time;

  name_action_map[idx].action_name = "start_time";
  name_action_map[idx++].action = start_time;

  name_action_map[idx].action_name = "stop_time";
  name_action_map[idx++].action = stop_time;

  name_action_map[idx].action_name = "get_todays_time";
  name_action_map[idx++].action = get_todays_time;

  name_action_map[idx].action_name = "get_overtime";
  name_action_map[idx++].action = get_overtime;

  name_action_map[idx].action_name = "get_work_in_progress";
  name_action_map[idx++].action = get_work_in_progress;

  name_action_map[idx].action_name = "get_dashboard";
  name_action_map[idx++].action = get_dashboard;

  name_action_map[idx].action_name = "post_dashboard";
  name_action_map[idx++].action = post_dashboard;
}

void deinit_name_action_map() { free(name_action_map); }

response_t route(request_t req, config_t* config) {
  LOGGER(TRACE, "Routing request with path=%s and method=%s\n", req.path, req.method);

  init_name_action_map();

#ifdef AUTH
  // Validate authorization header
  if (req.headers.authorization == NULL || config == NULL || config->bearer == NULL ||
      strcmp(req.headers.authorization, config->bearer) != 0) {
    // Respond with 403
    LOGGER(DEBUG, "Request with token %s forbidden\n", req.headers.authorization);
    response_t response;
    response.status_code = 403;
    response.status_message = "Forbidden";
    response.content_length = 0;
    response.content_type = "";
    response.body = NULL;
    response.location = "";

    return response;
  }
#endif  // AUTH

  // Iterate over the routes parsed from the routes.ini file to find a route
  // matching the request
  for (int i = 0; i < MAX_ROUTE_ARRAY_SIZE; ++i) {
    if (routes[i].name == NULL) {
      break;
    }
    if (strcmp(req.path, routes[i].path) == 0 && strcmp(req.method, routes[i].method) == 0) {
      LOGGER(TRACE, "Found matching route!\n", "");

      // Iterate over the name_action_map to find a matching implementation
      for (int j = 0; j < MAX_ROUTE_ARRAY_SIZE; ++j) {
        if (name_action_map[j].action_name == NULL) {
          break;
        }
        if (strcmp(routes[i].action, name_action_map[j].action_name) == 0) {
          LOGGER(TRACE, "Found matching implmentation!\n", "");
          response_t response = name_action_map[j].action(req);
          deinit_name_action_map();
          return response;
        }
      }

      LOGGER(ERROR, "No implementation of %s found!\n", routes[i].action);
    }
  }

  deinit_name_action_map();

  // Respond with 404
  LOGGER(DEBUG, "No matching route found\n", "");
  response_t response;
  response.status_code = 404;
  response.status_message = "Not found";
  response.content_length = 0;
  response.content_type = "";
  response.body = NULL;
  response.location = "";

  return response;
}
