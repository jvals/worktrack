#include <logger.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "compensation_repository.h"
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
  time_entry.todate = 0;  // Is recorded as <null> in database

  safe_new_entry(time_entry);
}

void time_service_stop_latest_entry() {
  LOGGER(INFO, "Setting TODATE on most recent entry\n", "");

  time_entry_t time_entry;
  time_t now;
  time(&now);
  LOGGER(TRACE, "Recording current timestamp=%d\n", now);
  time_entry.fromdate = 0;  // Don't care
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

  uint64_t unique_days_without_today = 0;
  get_unique_dates_without_today(&unique_days_without_today);

  uint64_t total_diff_without_today = 0;
  get_total_diff_without_today(&total_diff_without_today);

  return total_diff_without_today - (unique_days_without_today * 27000);  // 27000 = 7.5 * 60 * 60
}

bool time_service_unfinished_work() {
  LOGGER(INFO, "Checking if there are null values in the todate column of the time table\n", "");

  bool any_unfinished_work = false;
  check_unfinished_work(&any_unfinished_work);

  return any_unfinished_work;
}

uint64_t time_service_get_overtime_with_compensation() {
  uint64_t overtime = time_service_get_overtime();

  uint64_t total_compensation = 0;
  compensation_get_sum(&total_compensation);

  return overtime - total_compensation;
}

void time_service_get_all_work(char** all_work) {
  time_repository_get_all_work(all_work);
}

void time_service_update_work_start(char* id, char* new_work_start) {
  time_repository_update_work_start(id, new_work_start);
}
void time_service_update_work_end(char* id, char* new_work_end) {
  time_repository_update_work_end(id, new_work_end);
}
