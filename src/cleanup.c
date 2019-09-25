#include "cleanup.h"
#include "db_utils.h"
#include "routes_parser.h"

void cleanup() {
  deinit_routes();
  close_db_connection();
}
