#include <time.h>

// CMOCKA Headers
#include <stdbool.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/time_entry_t.h"
#include "../src/time_service.h"

void test_time_service_create_entry(void **state) {
  time_t now = time(NULL);
  time_entry_t time_entry_now = {.fromdate = now, .todate = 0};
  expect_memory(__wrap_safe_new_entry, &time_entry, &time_entry_now, sizeof(time_entry_t));

  time_service_create_entry();
}

void test_time_service_stop_latest_entry(void **state) {
  time_t now = time(NULL);
  time_entry_t time_entry_now = {.fromdate = 0, .todate = now};
  expect_memory(__wrap_patch_latest, &time_entry, &time_entry_now, sizeof(time_entry_t));

  time_service_stop_latest_entry();
}

void test_time_service_get_total_of_diffs() {

}

void __wrap_safe_new_entry(time_entry_t time_entry) {
  check_expected(&time_entry);
  return;
}

void __wrap_patch_latest(time_entry_t time_entry) {
  check_expected(&time_entry);
  return;
}

void  __wrap_get_total_diff(uint64_t* diff) {
  return;
}

void __wrap_logger() {
  return;
}


time_t __wrap_time(time_t *tloc) {
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

  const struct CMUnitTest tests[] = {
                                     cmocka_unit_test(test_time_service_create_entry),
                                     cmocka_unit_test(test_time_service_stop_latest_entry)
  };
  return cmocka_run_group_tests(tests, NULL, NULL);

  // test_time_service_create_entry();
  // test_time_service_stop_latest_entry();
  // test_time_service_get_total_of_diffs();

  return 0;
}
