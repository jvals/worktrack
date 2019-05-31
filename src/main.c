#include <logger.h>
#include <signal.h>
#include <stdlib.h>

#include "server.h"
#include "init.h"
#include "cleanup.h"

void handle_sigint(int sig) {
  cleanup();
  exit(0);
}

int main() {
  int err = 0;
  server_t server = {0};

  init();

  signal(SIGINT, handle_sigint);

  err = server_listen(&server);  // Open TCP Socket
  if (err) {
    LOGGER(FATAL, "Failed to listen on address 0.0.0.0:%d\n", PORT);
    return err;
  } else {
    LOGGER(INFO, "Successfully opened TCP socket on address 0.0.0.0:%d\n", PORT);
  }

  while (1) {
    err = server_accept(&server); // Check TCP Socket for connections
    if (err) {
      LOGGER(FATAL, "Failed accepting connection\n", "");
      return err;
    }
  }

  cleanup();

  return 0;
}
