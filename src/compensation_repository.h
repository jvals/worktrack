#pragma once

#include <stdint.h>

#include "compensation_entry_t.h"

void create_compensation_table(void);
void compensation_create_entry(compensation_entry_t);
void compensation_get_sum(uint64_t*);
