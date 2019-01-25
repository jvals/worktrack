#include "cleanup.h"
#include "routes_parser.h"
#include "db_utils.h"

void cleanup() {
  deinit_routes();
  close_db_connection();
}
