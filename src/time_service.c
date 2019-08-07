#include <logger.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

#include "time_entry_t.h"
#include "time_repository.h"
#include "time_service.h"

void time_service_create_entry() {
  LOGGER(INFO, "Creating new time entry\n", "");

  time_entry_t time_entry;
  time_t now;
  time(&now);
  LOGGER(TRACE, "Recording current timestamp=%d\n", now);

  time_entry.fromdate = now;
  time_entry.todate = 0; // Is recorded as <null> in database

  safe_new_entry(time_entry);
}

void time_service_stop_latest_entry() {
  LOGGER(INFO, "Setting TODATE on most recent entry\n", "");

  time_entry_t time_entry;
  time_t now;
  time(&now);
  LOGGER(TRACE, "Recording current timestamp=%d\n", now);
  time_entry.fromdate = 0; // Don't care
  time_entry.todate = now;

  patch_latest(time_entry);
  LOGGER(INFO, "Latest entry was updated with a TODATE\n", "");
}

uint64_t time_service_get_total_of_diffs() {
  LOGGER(INFO, "Computing the sum of all the diffs in the time table\n", "");

  uint64_t total_diff = 0;
  get_total_diff(&total_diff);
  LOGGER(INFO, "Total diff successfully computed=%d\n", total_diff);

  return total_diff;
}

uint64_t time_service_get_todays_diff() {
  LOGGER(INFO, "Computing the sum of todays diffs in the time table\n", "");

  uint64_t todays_diff = 0;
  get_todays_diff(&todays_diff);
  LOGGER(INFO, "Todays diff successfully computed=%d\n", todays_diff);

  uint64_t todays_unfinished_work = 0;
  get_todays_unfinished_work(&todays_unfinished_work);
  LOGGER(INFO, "Todays unfinished work successfully computed=%d\n", todays_unfinished_work);

  return todays_diff + todays_unfinished_work;
}

uint64_t time_service_get_overtime() {
  LOGGER(INFO, "Computing total overtime\n", "");

  // BUG: This number is too large if we are ignoring the current work day,
  // as the current work day can have multiple finished entries and one unfinished entry.
  uint64_t total_diff = time_service_get_total_of_diffs();

  uint64_t unique_days = 0;
  get_unique_dates(&unique_days);

  bool any_unfinished_work = time_service_unfinished_work();
  if (any_unfinished_work) {
    unique_days--; // Remove active workday
  }

  if (unique_days > 0) {
    return total_diff  - (unique_days * 27000); // 27000 = 7.5 * 60 * 60
  } else {
    LOGGER(INFO, "No workdays in database!\n", "");
    return 0;
  }
}

bool time_service_unfinished_work() {
  LOGGER(INFO, "Checking if there are null values in the todate column of the time table\n", "");

  bool any_unfinished_work = false;
  check_unfinished_work(&any_unfinished_work);

  return any_unfinished_work;
}
