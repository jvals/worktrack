#include <stdlib.h>
#include <logger.h>

#include "db_utils.h"

#define DATABASE_FILE_NAME "test.db"

sqlite3 *db = NULL;

int open_db_connection() {
  int rc = 0;

  rc = sqlite3_open(DATABASE_FILE_NAME, &db);

  if (rc) {
    LOGGER(FATAL, "Could not open database: %s\n", sqlite3_errmsg(db));
    return 1;
  } else {
    LOGGER(INFO, "Database with name %s was successfully opened\n", DATABASE_FILE_NAME);
    return 0;
  }
}

int close_db_connection() {
  if (db == NULL) {
    LOGGER(DEBUG, "Attemted to close db pointing at NULL\n", "");
  } else {
    int rc = sqlite3_close(db);
    if (rc == SQLITE_OK) {
      LOGGER(INFO, "Database with name %s was successfully closed\n", DATABASE_FILE_NAME);
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
