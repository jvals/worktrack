#include "config_t.h"

#define PORT 8080

typedef struct server {
  int listen_fd;
} server_t;

int server_accept(server_t*, config_t*);
int server_listen(server_t*);
