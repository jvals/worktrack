#include <errno.h>
#include <string.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>

#include "config_t.h"

config_t* parse_config() {
  const char* config_file_name = "config.ini";
  FILE* fp = NULL;
  if ( (fp = fopen(config_file_name, "r")) == NULL ) {
      LOGGER(FATAL, "Unable to read file <%s>. fopen failed with error: %s\n", config_file_name, strerror(errno));
      exit(1);
  } else {
      LOGGER(INFO, "Reading file %s\n", config_file_name);
  }

  config_t* config = malloc(sizeof(config_t));

  char *line, *line_to_free;
  line = line_to_free = malloc(128 * sizeof(char));
  while(fgets(line, 128, fp) != NULL) {
    char* token = strsep(&line, "=");
    if (strcmp(token, "authorization") == 0) {
      config->bearer = strdup(line);
    }
  }

  free(line_to_free);

  return config;
}
