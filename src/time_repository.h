#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "time_entry_t.h"

void create_time_table(void);
void new_entry(time_entry_t);
void patch_latest(time_entry_t);
void get_total_diff(uint64_t *total);
void safe_new_entry(time_entry_t);
void get_todays_diff(uint64_t *total);
void get_todays_unfinished_work(uint64_t *total);
void get_unique_dates(uint64_t *count);
void check_unfinished_work(bool *);
void get_unique_dates_without_today(uint64_t *count);
void get_total_diff_without_today(uint64_t *total);
void time_repository_get_all_work(char **all_work);
void time_repository_update_work_start(char* id, char* new_work_start);
void time_repository_update_work_end(char* id, char* new_work_end);
