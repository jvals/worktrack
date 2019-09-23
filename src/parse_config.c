#include <errno.h>
#include <string.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>

#include "config_t.h"

static config_t* default_config() {
  config_t* config = malloc(sizeof(config_t));
  config->bearer = malloc(sizeof(char)*5);
  sprintf(config->bearer, "1234");
  return config;
}

config_t* parse_config() {
  const char* config_file_name = "/var/config/config.ini";
  FILE* fp = NULL;
  if ( (fp = fopen(config_file_name, "r")) == NULL ) {
      LOGGER(WARN, "Unable to read file <%s>. fopen failed with error: %s\n", config_file_name, strerror(errno));
      LOGGER(WARN, "Using default config\n", "");
      return default_config();
  } else {
      LOGGER(INFO, "Reading file %s\n", config_file_name);
  }

  config_t* config = malloc(sizeof(config_t));

  char *line, *line_to_free;
  line = line_to_free = malloc(128 * sizeof(char));
  while(fgets(line, 128, fp) != NULL) {
    line[strcspn(line, "\n")] = '\0'; // Replace the newline with a line end
    char* token = strsep(&line, "=");
    if (strcmp(token, "authorization") == 0) {
      config->bearer = strdup(line);
    }
  }

  free(line_to_free);

  return config;
}
