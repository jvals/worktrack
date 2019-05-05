#include <time.h>

#include "../src/time_entry_t.h"

void test_time_service_create_entry() {

}

void test_time_service_stop_latest_entry() {

}

void test_time_service_get_total_of_diffs() {

}

void __wrap_safe_new_entry(time_entry_t time_entry) {
  return;
}

time_t time(time_t *tloc) {
  struct tm may1st;
  may1st.tm_sec = 0;
  may1st.tm_min = 0;
  may1st.tm_hour = 0;
  may1st.tm_mday = 1;
  may1st.tm_mon = 4;
  may1st.tm_year = 2019 - 1900;
  may1st.tm_isdst = 1;

  if (tloc != NULL) {
    *tloc = mktime(&may1st);
  }

  return mktime(&may1st);
}

int  main() {
  test_time_service_create_entry();
  test_time_service_stop_latest_entry();
  test_time_service_get_total_of_diffs();

  return 0;
}
