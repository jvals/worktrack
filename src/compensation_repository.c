#include <logger.h>
#include <stdlib.h>
#include <stdio.h>


#include "db_utils.h"

#define COMPENSATION_TABLE_NAME "compensation"

static int create_compensation_table_callback(void *NotUsed, int argc, char **argv, char **azColName) {
  return 0;
}

void create_compensation_table() {
  sqlite3* db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "CREATE TABLE IF NOT EXISTS %s (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, DURATION INT NOT NULL, CREATED INT NOT NULL);", COMPENSATION_TABLE_NAME);

    char* error = NULL;
    int rc = sqlite3_exec(db, sql_raw, create_compensation_table_callback, 0, &error);
    if ( rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to create table: %s\n", error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(INFO, "Table with name '%s' created successfully\n", COMPENSATION_TABLE_NAME);
    }
  }
}
