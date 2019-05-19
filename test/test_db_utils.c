#include <sqlite3.h>
#include <string.h>

// CMOCKA Headers
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/db_utils.h"

void test_open_db_connection(void **state) {
  (void)state;

  int rc = open_db_connection();
  assert_return_code(rc, 0);
}

void test_close_db_connection(void **state) {

}

int __wrap_sqlite3_open(const char *filename, sqlite3 *ppDb) {
  return strcmp(filename, DATABASE_FILE_NAME);
}

void __wrap_logger() {
  return;
}

int main() {
  const struct CMUnitTest tests[] = {
                                     cmocka_unit_test(test_open_db_connection)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
