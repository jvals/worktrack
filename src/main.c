#include <logger.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#include "server.h"
#include "init.h"
#include "cleanup.h"

#define UNUSED(x) (void)(x)

void handle_sigint(int sig) {
  UNUSED(sig);
  cleanup();
  exit(0);
}

int main() {
  int err = 0;
  server_t server = {0};

  init();
  setvbuf(stdout, NULL, _IONBF, 0); // Always flush stdout

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
