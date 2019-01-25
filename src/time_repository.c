#include <stdlib.h>
#include <logger.h>
#include <stdio.h>

#include "db_utils.h"

#define TIME_TABLE_NAME "time"

int create_time_table_callback(void *NotUsed, int argc, char **argv, char **azColName) {
  LOGGER(DEBUG, "Creating time table\n","");
  for (int i = 0; i < argc; ++i) {
    LOGGER(TRACE, "%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void create_time_table() {
  sqlite3* db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "CREATE TABLE IF NOT EXISTS %s (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, FROMDATE INT NOT NULL, TODATE INT);", TIME_TABLE_NAME);

    char* error = NULL;
    int rc = sqlite3_exec(db, sql_raw, create_time_table_callback, 0, &error);
    if ( rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to create table: %s\n", error);
      exit(1);
    } else {
      LOGGER(INFO, "Table created successfully\n", "");
    }
  }
}
