#include <stdlib.h>
#include <logger.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "time_entry_t.h"
#include "db_utils.h"
#include "time_repository.h"

#define TIME_TABLE_NAME "time"

int time_callback(void *NotUsed, int argc, char **argv, char **azColName) {
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
    int rc = sqlite3_exec(db, sql_raw, time_callback, 0, &error);
    if ( rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to create table: %s\n", error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(INFO, "Table created successfully\n", "");
    }
  }
}

// Make sure that the most recent entry has todate!=null before creating a new entry
void safe_new_entry(time_entry_t time_entry) {
  sqlite3* db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    if (time_entry.todate == 0) {
      sprintf(sql_raw,
              "INSERT INTO %s (FROMDATE) SELECT %d WHERE (SELECT todate from time ORDER BY id DESC LIMIT 1) IS NOT NULL;",
              TIME_TABLE_NAME,
              time_entry.fromdate);
    } else {
      sprintf(sql_raw,
              "INSERT INTO %s (FROMDATE, TODATE) SELECT %d, %d WHERE (SELECT todate from time ORDER BY id DESC LIMIT 1) IS NOT NULL;",
              TIME_TABLE_NAME,
              time_entry.fromdate,
              time_entry.todate);
    }

    char* error = NULL;
    int rc = sqlite3_exec(db, sql_raw, time_callback, 0, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to insert into %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(INFO, "New time entry successfully created\n", ""); // Not always true
    }
  }
}

void patch_latest(time_entry_t time_entry) {
  sqlite3* db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "UPDATE %s SET TODATE=%d ORDER BY id DESC LIMIT 1;", TIME_TABLE_NAME, time_entry.todate);
    char* error = NULL;
    int rc = sqlite3_exec(db, sql_raw, time_callback, 0, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to update table %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Time entry was updated!\n", "");
    }
  }
}

int get_total_sum_callback(void *total, int argc, char **argv, char **azColName) {
  LOGGER(TRACE, "TODATE-FROMDATE sum computed to %s\n", argv[0]);
  if (argv[0] != NULL) {
    *((uint64_t*)total) = strtol(argv[0], NULL, 10);
  }
  return 0;
}

void get_total_diff(uint64_t *total) {
  sqlite3* db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "SELECT SUM(TODATE-FROMDATE) FROM %s", TIME_TABLE_NAME);
    char* error = NULL;
    int rc = sqlite3_exec(db, sql_raw, get_total_sum_callback, total, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to get total %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Successfully collected the total time spent!\n", "");
    }
  }
}
