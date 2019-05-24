#pragma once

#include "response.h"
#include "request.h"

response_t get_total_time(request_t);
response_t start_time(request_t);
response_t stop_time(request_t);
response_t get_todays_time(request_t);
response_t get_overtime(request_t);
response_t get_work_in_progress(request_t);
