#include <logger.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cleanup.h"
#include "config_t.h"
#include "db_utils.h"
#include "init.h"
#include "parse_config.h"
#include "server.h"

#define UNUSED(x) (void)(x)

void handle_sigint(int sig) {
  UNUSED(sig);
  cleanup();
  exit(0);
}

void parse_opts(int argc, char* argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "d:")) != -1) {
    switch (opt) {
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

  // Parse configuration file
  config_t* config = parse_config();

  init();
  setvbuf(stdout, NULL, _IONBF, 0);  // Always flush stdout

  signal(SIGINT, handle_sigint);

  err = server_listen(&server);  // Open TCP Socket
  if (err) {
    LOGGER(FATAL, "Failed to listen on address 0.0.0.0:%d\n", PORT);
    return err;
  } else {
    LOGGER(INFO, "Successfully opened TCP socket on address 0.0.0.0:%d\n", PORT);
  }

  while (1) {
    err = server_accept(&server, config);  // Check TCP Socket for connections
    if (err) {
      LOGGER(FATAL, "Failed accepting connection\n", "");
      return err;
    }
  }

  free(config);
  cleanup();

  return 0;
}
