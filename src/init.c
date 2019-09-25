#include "compensation_repository.h"
#include "db_utils.h"
#include "routes_parser.h"
#include "time_repository.h"

void init() {
  open_db_connection();
  init_routes_from_ini();
  create_time_table();
  create_compensation_table();
}
