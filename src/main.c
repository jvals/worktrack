#include <logger.h>

#include "server.h"

int main() {
  int err = 0;
  server_t server = {0};

  err = server_listen(&server);  // Open TCP Socket
  if (err) {
    LOGGER(FATAL, "Failed to listen on address 0.0.0.0:%d\n", PORT);
    return err;
  }

  while (1) {
    err = server_accept(&server); // Check TCP Socket for connections
    if (err) {
      LOGGER(FATAL, "Failed accepting connection\n", "");
      return err;
    }
  }

  return 0;
}
