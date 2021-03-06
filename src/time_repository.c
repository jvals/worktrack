#include <inttypes.h>
#include <logger.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "db_utils.h"
#include "time_entry_t.h"
#include "time_repository.h"

#define TIME_TABLE_NAME "time"
#define UNUSED(x) (void)(x)

static int time_callback(void *NotUsed, int argc, char **argv, char **azColName) {
  UNUSED(NotUsed);
  for (int i = 0; i < argc; ++i) {
    LOGGER(TRACE, "%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void create_time_table() {
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw,
            "CREATE TABLE IF NOT EXISTS %s (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, FROMDATE INT NOT NULL, "
            "TODATE INT);",
            TIME_TABLE_NAME);

    char *error = NULL;
    int rc = sqlite3_exec(db, sql_raw, time_callback, 0, &error);
    if (rc != SQLITE_OK) {
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
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    if (time_entry.todate == 0) {
      sprintf(sql_raw,
              "INSERT INTO %s (FROMDATE) SELECT %d WHERE (SELECT 1 from time limit 1) IS NULL OR (SELECT todate from "
              "time ORDER BY id DESC LIMIT 1) IS NOT NULL;",
              TIME_TABLE_NAME, time_entry.fromdate);
    } else {
      sprintf(sql_raw,
              "INSERT INTO %s (FROMDATE, TODATE) SELECT %d, %d WHERE (SELECT 1 from time limit 1) IS NULL OR (SELECT "
              "todate from time ORDER BY id DESC LIMIT 1) IS NOT NULL;",
              TIME_TABLE_NAME, time_entry.fromdate, time_entry.todate);
    }

    char *error = NULL;
    int rc = sqlite3_exec(db, sql_raw, time_callback, 0, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to insert into %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(INFO, "New time entry successfully created\n", "");  // Not always true
    }
  }
}

void patch_latest(time_entry_t time_entry) {
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "UPDATE %1$s SET TODATE=%2$d WHERE id=(SELECT MAX(id) FROM %1$s);", TIME_TABLE_NAME,
            time_entry.todate);
    char *error = NULL;
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

static int get_total_sum_callback(void *total, int argc, char **argv, char **azColName) {
  UNUSED(argc);
  UNUSED(azColName);
  LOGGER(TRACE, "TODATE-FROMDATE sum computed to %s\n", argv[0]);
  if (argv[0] != NULL) {  // argv[0] is 0x0 if the time table is empty
    *((uint64_t *)total) = strtol(argv[0], NULL, 10);
  }
  return 0;
}

void get_total_diff(uint64_t *total) {
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "SELECT SUM(TODATE-FROMDATE) FROM %s", TIME_TABLE_NAME);
    char *error = NULL;
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

static int get_todays_sum_callback(void *total, int argc, char **argv, char **azColName) {
  UNUSED(argc);
  UNUSED(azColName);
  LOGGER(TRACE, "Todays time computed to %s\n", argv[0]);
  if (argv[0] != NULL) {  // argv[0] is 0x0 if the time table is empty
    *((uint64_t *)total) = strtol(argv[0], NULL, 10);
  }
  return 0;
}

void get_todays_diff(uint64_t *total) {
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw,
            "SELECT SUM(TODATE-FROMDATE) FROM %s where fromdate >= strftime('%%s', 'now', 'start of day') and todate "
            ">= strftime('%%s', 'now', 'start of day')",
            TIME_TABLE_NAME);
    char *error = NULL;
    int rc = sqlite3_exec(db, sql_raw, get_todays_sum_callback, total, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to get total %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Successfully collected todays time spent!\n", "");
    }
  }
}

static int get_todays_unfinished_work_callback(void *total, int argc, char **argv, char **azColName) {
  UNUSED(azColName);
  if (argc >= 2 && argv[0] != NULL && argv[1] == NULL) {
    time_t now;
    time(&now);
    *((uint64_t *)total) = ((uint64_t)now - strtol(argv[0], NULL, 10));
  }
  return 0;
}

void get_todays_unfinished_work(uint64_t *total) {
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw,
            "SELECT FROMDATE, TODATE from %s where fromdate >= strftime('%%s', 'now', 'start of day') order by id desc "
            "limit 1",
            TIME_TABLE_NAME);
    char *error = NULL;
    int rc = sqlite3_exec(db, sql_raw, get_todays_unfinished_work_callback, total, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to get total %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Successfully collected todays unfinished time!\n", "");
    }
  }
}

static int get_unique_dates_callback(void *count, int argc, char **argv, char **azColName) {
  UNUSED(azColName);
  if (argc >= 0) {
    LOGGER(INFO, "Unique dates in time table: %s\n", argv[0]);
    *((uint64_t *)count) = strtol(argv[0], NULL, 10);
  }
  return 0;
}

void get_unique_dates(uint64_t *count) {
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "SELECT COUNT(DISTINCT DATE(fromdate, 'unixepoch')) from %s;", TIME_TABLE_NAME);
    char *error = NULL;
    int rc = sqlite3_exec(db, sql_raw, get_unique_dates_callback, count, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to get unique dates %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Successfully collected unique dates!\n", "");
    }
  }
}

static int check_unfinished_work_callback(void *unfinished_work, int argc, char **argv, char **azColName) {
  UNUSED(azColName);
  if (argc >= 0) {
    LOGGER(INFO, "Number of rows where todate is NULL: %s\n", argv[0]);
    *((bool *)unfinished_work) = (strtol(argv[0], NULL, 10) > 0);
  }
  return 0;
}

void check_unfinished_work(bool *unfinished_work) {
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "SELECT COUNT(*) FROM %s WHERE todate IS NULL", TIME_TABLE_NAME);
    char *error = NULL;
    int rc = sqlite3_exec(db, sql_raw, check_unfinished_work_callback, unfinished_work, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to check unfinished work %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Successfully checked unfinished work!\n", "");
    }
  }
}

void get_unique_dates_without_today(uint64_t *count) {
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw,
            "SELECT COUNT(DISTINCT DATE(fromdate, 'unixepoch')) from %s where fromdate < strftime('%%s', 'now', 'start "
            "of day')",
            TIME_TABLE_NAME);
    char *error = NULL;
    int rc = sqlite3_exec(db, sql_raw, get_unique_dates_callback, count, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to get unique dates %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Successfully collected unique dates!\n", "");
    }
  }
}

void get_total_diff_without_today(uint64_t *total) {
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {
    char sql_raw[1024];
    sprintf(sql_raw, "SELECT SUM(TODATE-FROMDATE) FROM %s where fromdate < strftime('%%s', 'now', 'start of day')",
            TIME_TABLE_NAME);
    char *error = NULL;
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

static int get_size_of_all_work_callback(void *result_size, int argc, char **argv, char **azColName) {
  UNUSED(azColName);

  for(int i = 0; i < argc; i++) {
    if (argv[i] != NULL) {
      *((size_t *)result_size) += strlen(argv[i]);
    }
  }

  *((size_t *)result_size) += 16; // newline

  return 0;

}

static int get_all_work_callback(void *all_work, int argc, char **argv, char **azColName) {
  UNUSED(azColName);

  for(int i = 0; i < argc; i++) {
    if (argv[i] != NULL) {
      strcat((char *)all_work, argv[i]);
      if (i < argc - 1) {
        strcat((char *)all_work, ", ");
      }
    }
  }

  strcat((char *)all_work, "\n");

  return 0;
}

void time_repository_get_all_work(char **all_work) {
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {

    // Compute size of result set
    size_t result_size = 0;
    char sql_raw[1024];
    sprintf(sql_raw, "select * from %s", TIME_TABLE_NAME);
    char *error = NULL;
    int rc = sqlite3_exec(db, sql_raw, get_size_of_all_work_callback, &result_size, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to get size of all the work from table %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Successfully collected size of all the work!\n", "");
    }

    // Get all the work
    *all_work = calloc(1, result_size);
    error = NULL;
    rc = sqlite3_exec(db, sql_raw, get_all_work_callback, *all_work, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to get all the work from table %s: %s\n", TIME_TABLE_NAME, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Successfully collected all the work!\n", "");
    }
  }
}

void time_repository_update_work_start(char* id, char* new_work_start) {
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {

    char sql_raw[1024];
    sprintf(sql_raw, "update %s set fromdate=%s where id=%s", TIME_TABLE_NAME, new_work_start, id);
    char *error = NULL;
    int rc = sqlite3_exec(db, sql_raw, NULL, NULL, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to update fromdate to %s on work with id %s: %s\n", new_work_start, id, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Successfully set fromdate to %s on work with id %s\n", new_work_start, id);
    }
  }
}

void time_repository_update_work_end(char* id, char* new_work_end) {
  sqlite3 *db = getDb();
  if (db == NULL) {
    LOGGER(FATAL, "Unable to get database pointer\n", "");
    exit(1);
  } else {

    char sql_raw[1024];
    sprintf(sql_raw, "update %s set todate=%s where id=%s", TIME_TABLE_NAME, new_work_end, id);
    char *error = NULL;
    int rc = sqlite3_exec(db, sql_raw, NULL, NULL, &error);
    if (rc != SQLITE_OK) {
      LOGGER(FATAL, "Unable to update todate to %s on work with id %s: %s\n", new_work_end, id, error);
      sqlite3_free(error);
      exit(1);
    } else {
      LOGGER(DEBUG, "Successfully set todate to %s on work with id %s\n", new_work_end, id);
    }
  }
}
