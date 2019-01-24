#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <logger.h>
#include <stdlib.h>

#include "routes_parser.h"

#define ROUTES_FILE_NAME "routes.ini"
#define MAX_CONFIG_LINE_SIZE 64

// Global route list
route_t* routes;

// This function modifies the global variable 'routes'
void init_routes_from_ini() {
  // Open file
  FILE* fp = NULL;
  if ( (fp = fopen(ROUTES_FILE_NAME, "r")) == NULL ) {
    LOGGER(FATAL, "Unable to read file <%s>. fopen failed with error: %s\n", ROUTES_FILE_NAME, strerror(errno));
    exit(1);
  } else {
    LOGGER(INFO, "Reading file %s\n", ROUTES_FILE_NAME);
  }

  routes = malloc(sizeof(route_t) * MAX_ROUTE_ARRAY_SIZE);
  char line[MAX_CONFIG_LINE_SIZE];
  int routes_idx = 0;
  while(fgets(line, MAX_CONFIG_LINE_SIZE, fp) != NULL ) {
    // Match section name
    if (line[0] == '[') {
      routes[routes_idx].name = malloc(MAX_CONFIG_LINE_SIZE*sizeof(char));
      // Copy char by char to discard '[' and ']'
      for(int i = 1; i < MAX_CONFIG_LINE_SIZE; i++) {
        if (line[i] != ']') {
          routes[routes_idx].name[i-1] = line[i];
        } else {
          routes[routes_idx].name[i-1] = '\0';
          break;
        }
      }

      // Path
      LOGGER(TRACE, "Reading path for route %s\n", routes[routes_idx].name);
      char* original_line;
      char* path_line = original_line = malloc(MAX_CONFIG_LINE_SIZE * sizeof(char));
      char* err = fgets(path_line, MAX_CONFIG_LINE_SIZE, fp);
      if(err != NULL) {
        strsep(&path_line, "=");
        routes[routes_idx].path = strdup(path_line);
        // Remove trailing newline
        routes[routes_idx].path[strcspn(routes[routes_idx].path, "\n")] = 0;
      } else {
        LOGGER(FATAL, "Error reading from %s. %s", ROUTES_FILE_NAME, strerror(errno));
      }
      free(original_line);
      LOGGER(TRACE, "Successfully read path=%s\n", routes[routes_idx].path);

      // Method
      LOGGER(TRACE, "Reading method for route %s\n", routes[routes_idx].name);
      char* method_line = original_line = malloc(MAX_CONFIG_LINE_SIZE * sizeof(char));
      err = fgets(method_line, MAX_CONFIG_LINE_SIZE, fp);
      if(err != NULL) {
        strsep(&method_line, "=");
        routes[routes_idx].method = strdup(method_line);
        // Remove trailing newline
        routes[routes_idx].method[strcspn(routes[routes_idx].method, "\n")] = 0;
      } else {
        LOGGER(FATAL, "Error reading from %s. %s", ROUTES_FILE_NAME, strerror(errno));
      }
      free(original_line);
      LOGGER(TRACE, "Successfully read method=%s\n", routes[routes_idx].method);

      // Action
      LOGGER(TRACE, "Reading action for route %s\n", routes[routes_idx].name);
      char* action_line = original_line = malloc(MAX_CONFIG_LINE_SIZE * sizeof(char));
      err = fgets(action_line, MAX_CONFIG_LINE_SIZE, fp);
      if(err != NULL) {
        strsep(&action_line, "=");
        routes[routes_idx].action = strdup(action_line);
        // Remove trailing newline
        routes[routes_idx].action[strcspn(routes[routes_idx].action, "\n")] = 0;
      } else {
        LOGGER(FATAL, "Error reading from %s. %s", ROUTES_FILE_NAME, strerror(errno));
      }
      free(original_line);
      LOGGER(TRACE, "Successfully read action=%s\n", routes[routes_idx].action);

      routes_idx++;
    }
  }

  fclose(fp);
}

void deinit_routes() {
  for (int i = 0; i < MAX_ROUTE_ARRAY_SIZE; ++i) {
    if (routes[i].name != NULL) {
      free(routes[i].name);
    }

    if (routes[i].path != NULL) {
      free(routes[i].path);
    }

    if (routes[i].method != NULL) {
      free(routes[i].method);
    }

    if (routes[i].action != NULL) {
      free(routes[i].action);
    }
  }

  free(routes);
}
