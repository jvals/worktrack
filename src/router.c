#include "router.h"

#include "time_controller.h"

typedef struct route {
  char* path;
  char* method;
  char* action;
} route_t;

response_t route(request_t req) {
  // TODO: Populate a route array from routes.ini
  route_t routes[10];
  routes[0].path = "/time";
  routes[0].method = "POST";
  routes[0].action = "start_time";

  return start_time(req);
}
