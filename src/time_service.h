#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "time_entry_t.h"

void time_service_create_entry(void);
void time_service_stop_latest_entry(void);
uint64_t time_service_get_total_of_diffs(void);
uint64_t time_service_get_todays_diff(void);
uint64_t time_service_get_overtime(void);
bool time_service_unfinished_work(void);
uint64_t time_service_get_overtime_with_compensation(void);
