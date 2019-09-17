#include <logger.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "server.h"
#include "init.h"
#include "cleanup.h"
#include "db_utils.h"

#define UNUSED(x) (void)(x)

void handle_sigint(int sig) {
  UNUSED(sig);
  cleanup();
  exit(0);
}

void parse_opts(int argc, char* argv[]) {
  int opt;
  while((opt = getopt(argc, argv, "d:")) != -1) {
    switch(opt) {
    case 'd':
      LOGGER(INFO, "Running server with database path %s\n", optarg);
      set_datebase_path(optarg);
      break;
    }
  }
}

int main(int argc, char* argv[]) {
  int err = 0;
  server_t server = {0};

  // Parse command line options
  parse_opts(argc, argv);

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
