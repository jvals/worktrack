#define PORT 8080

typedef struct server {
  int listen_fd;
} server_t;

int server_accept(server_t*);
int server_listen(server_t*);
