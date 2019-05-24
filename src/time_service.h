#pragma once
#include <stdint.h>
#include "time_entry_t.h"

void time_service_create_entry(void);
void time_service_stop_latest_entry(void);
uint64_t time_service_get_total_of_diffs(void);
uint64_t time_service_get_todays_diff(void);
