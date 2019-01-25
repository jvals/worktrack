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

bool check_last_entry_callback_was_executed = false;
int check_last_entry_callback(void *time_entry, int argc, char **argv, char **azColName) {
  check_last_entry_callback_was_executed = true;
  if (argv[0] == NULL) {
    LOGGER(TRACE, "TODATE in last entry in table %s was null\n", TIME_TABLE_NAME);
  } else {
    LOGGER(TRACE, "TODATE in last entry in table %s was %s\n", TIME_TABLE_NAME, argv[0]);
    new_entry(*((time_entry_t*)time_entry));
  }

  return 0;
}

void check_last_entry(time_entry_t time_entry) {
  sqlite3* db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "SELECT todate from %s ORDER BY id DESC LIMIT 1;", TIME_TABLE_NAME);
    char* error = NULL;
    int rc = sqlite3_exec(db, sql_raw, check_last_entry_callback, (void*)&time_entry, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to fetch last entry from table &s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      if (!check_last_entry_callback_was_executed) {
        new_entry(time_entry);
      }
      LOGGER(DEBUG, "Last entry was fetched!\n", "");
    }
  }
}

// Make sure that the most recent entry has todate!=null before creating a new entry
void safe_new_entry(time_entry_t time_entry) {
  check_last_entry(time_entry); // Calls new_entry in a callback if it's safe
}

void new_entry(time_entry_t time_entry) {
  sqlite3* db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    if (time_entry.todate == 0) {
      sprintf(sql_raw, "INSERT INTO %s (FROMDATE) VALUES (%d);", TIME_TABLE_NAME, time_entry.fromdate);
    } else {
      sprintf(sql_raw, "INSERT INTO %s (FROMDATE, TODATE) VALUES (%d, %d);", TIME_TABLE_NAME, time_entry.fromdate, time_entry.todate);
    }

    char* error = NULL;
    int rc = sqlite3_exec(db, sql_raw, time_callback, 0, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to insert into %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Time entry was created!\n", "");
      LOGGER(INFO, "New time entry successfully created\n", "");
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

uint64_t global_total = 0;
uint64_t todate_total = 0;
uint64_t fromdate_total = 0;

int get_todate_sum_callback(void *total, int argc, char **argv, char **azColName) {
  LOGGER(TRACE, "TODATE sum computed to %s\n", argv[0]);
  if (argv[0] != NULL) {
    todate_total = strtol(argv[0], NULL, 10);
  }
  return 0;
}

int get_fromdate_sum_callback(void *total, int argc, char **argv, char **azColName) {
  LOGGER(TRACE, "FROMDATE sum computed to %s\n", argv[0]);
  if (argv[0] != NULL) {
    fromdate_total = strtol(argv[0], NULL, 10);
  }
  return 0;
}

void get_total_fromdate() {
  sqlite3* db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "SELECT SUM(FROMDATE) FROM %s", TIME_TABLE_NAME);
    char* error = NULL;
    int rc = sqlite3_exec(db, sql_raw, get_fromdate_sum_callback, 0, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to get total fromdates %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Successfully collected the sum of fromdates!\n", "");
    }
  }
}

void get_total_todate() {
  sqlite3* db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "SELECT SUM(TODATE) FROM %s", TIME_TABLE_NAME);
    char* error = NULL;
    int rc = sqlite3_exec(db, sql_raw, get_todate_sum_callback, 0, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to get total todates %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Successfully collected the sum of todates!\n", "");
    }
  }
}

void get_total_diff(uint64_t *total) {
    get_total_fromdate();
    get_total_todate();
    LOGGER(TRACE, "Computing diff of %d and %d\n", todate_total, fromdate_total);
    global_total = (todate_total-fromdate_total);
    *total = global_total;
}
