#include <logger.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


#include "db_utils.h"
#include "compensation_entry_t.h"

#define COMPENSATION_TABLE_NAME "compensation"
#define UNUSED(x) (void)(x)

static int create_compensation_table_callback(void *NotUsed, int argc, char **argv, char **azColName) {
  UNUSED(NotUsed);
  for (int i = 0; i < argc; ++i) {
    LOGGER(TRACE, "%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
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

static int compensation_create_entry_callback(void *NotUsed, int argc, char **argv, char **azColName) {
  UNUSED(NotUsed);
  for (int i = 0; i < argc; ++i) {
    LOGGER(TRACE, "%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void compensation_create_entry(compensation_entry_t compensation_entry) {
  sqlite3* db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "INSERT INTO %s (DURATION, CREATED) values (%d, %d)", COMPENSATION_TABLE_NAME, compensation_entry.duration, compensation_entry.created);

    char* error = NULL;
    int rc = sqlite3_exec(db, sql_raw, compensation_create_entry_callback, 0, &error);
    if ( rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to create table: %s\n", error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(INFO, "Compensation entry successfully created\n", "");
    }
  }
}

static int compensation_get_sum_callback(void *total, int argc, char **argv, char **azColName) {
  UNUSED(argc);
  UNUSED(azColName);
  LOGGER(TRACE, "SUM(duration) computed to %s\n", argv[0]);
  if (argv[0] != NULL) { // argv[0] is 0x0 if the time table is empty
    *((uint64_t*)total) = strtol(argv[0], NULL, 10);
  }
  return 0;
}

void compensation_get_sum(uint64_t *total) {
  sqlite3* db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "SELECT SUM(DURATION) from %s", COMPENSATION_TABLE_NAME);

    char* error = NULL;
    int rc = sqlite3_exec(db, sql_raw, compensation_get_sum_callback, total, &error);
    if ( rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to create table: %s\n", error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(INFO, "Compensation sum successfully computed: %d\n", *total);
    }
  }
}
