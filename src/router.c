#include <stdlib.h>
#include <string.h>
#include <logger.h>

#include "router.h"
#include "time_controller.h"

typedef struct route {
  char* name;
  char* path;
  char* method;
  char* action;
} route_t;

typedef struct name_action_mapping {
  char* action_name;
  response_t (*action)(request_t);
} name_action_mapping_t;

name_action_mapping_t* name_action_map;

response_t route(request_t req) {
  LOGGER(TRACE, "Routing request with path=%s and method=%s\n", req.path, req.method);

  name_action_map = malloc(10 * sizeof(name_action_mapping_t));
  name_action_map[0].action_name = "start_time";
  name_action_map[0].action = start_time;

  // TODO: Populate a route array from routes.ini
  route_t routes[10];
  routes[0].name = "get_total_time";
  routes[0].path = "/time";
  routes[0].method = "GET";
  routes[0].action = "get_total_time";

  routes[1].name = "start_time";
  routes[1].path = "/time";
  routes[1].method = "POST";
  routes[1].action = "start_time";

  routes[2].name = "stop_time";
  routes[2].path = "/time";
  routes[2].method = "PATCH";
  routes[2].action = "stop_time";

  for (int i = 0; i < 3; ++i) {
    if (strcmp(req.path, routes[i].path)==0 && strcmp(req.method, routes[i].method)) {
      LOGGER(TRACE, "Found matching route!\n", "");
      for (int j = 0; j < 1; ++j) {
        if (strcmp(routes[i].action, name_action_map[j].action_name)) {
          LOGGER(TRACE, "Found matching implmentation!\n", "");
          response_t response = name_action_map[j].action(req);
          return response;
        }
      }

      LOGGER(ERROR, "No implementation of %s found!\n", routes[i].action);
    }
  }


  // Respond with 404
  LOGGER(DEBUG, "No mathing route found\n", "");
  response_t response;
  response.status_code = 404;
  response.status_message = "Not found";
  response.content_length = 0;
  response.content_type = "";
  response.body = "";

  return response;
}
