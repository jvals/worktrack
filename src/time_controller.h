#pragma once

#include "request.h"
#include "response.h"

response_t get_total_time(request_t);
response_t start_time(request_t);
response_t stop_time(request_t);
response_t get_todays_time(request_t);
response_t get_overtime(request_t);
response_t get_work_in_progress(request_t);
response_t get_all_work(request_t);
response_t update_work_start(request_t);
response_t update_work_end(request_t);
response_t update_work_start_options(request_t);
response_t update_work_end_options(request_t);
