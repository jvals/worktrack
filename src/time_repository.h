#pragma once

#include <stdint.h>

#include "time_entry_t.h"

void create_time_table(void);
void new_entry(time_entry_t);
void patch_latest(time_entry_t);
void get_total_diff(uint64_t *total);
void safe_new_entry(time_entry_t);
void get_todays_diff(uint64_t *total);
void get_todays_unfinished_work(uint64_t *total);
