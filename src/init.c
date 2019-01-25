#include "routes_parser.h"
#include "db_utils.h"


void init() {
  open_db_connection();
  init_routes_from_ini();
}
