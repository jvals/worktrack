#pragma once

typedef struct route {
  char* name;
  char* path;
  char* method;
  char* action;
} route_t;

extern route_t* routes;
