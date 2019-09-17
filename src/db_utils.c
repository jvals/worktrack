#include <stdlib.h>
#include <logger.h>
#include <string.h>

#include "db_utils.h"

sqlite3 *db = NULL;

char* database_path = NULL;

void set_datebase_path(char* new_path) {
  database_path = strdup(new_path);
}

int open_db_connection() {
  int rc = 0;

  if (database_path == NULL) {
    set_datebase_path("test.db");
  }

  rc = sqlite3_open(database_path, &db);

  if (rc) {
    LOGGER(FATAL, "Could not open database %s: %s\n", database_path, sqlite3_errmsg(db));
    return 1;
  } else {
    LOGGER(INFO, "Database with name %s was successfully opened\n", database_path);
    return 0;
  }
}

int close_db_connection() {
  if (db == NULL) {
    LOGGER(DEBUG, "Attemted to close db pointing at NULL\n", "");
  } else {
    int rc = sqlite3_close(db);
    if (rc == SQLITE_OK) {
      LOGGER(INFO, "Database with name %s was successfully closed\n", database_path);
      return 0;
    } else {
      LOGGER(ERROR, "Unable to close database. Transactions might be in progress\n", "");
    }
  }
  return 1;
}

sqlite3* getDb() {
  return db;
}
