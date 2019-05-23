#include <logger.h>
#include <time.h>
#include <stdint.h>

#include "time_entry_t.h"
#include "time_repository.h"

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

  return todays_diff;
}
