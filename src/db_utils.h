#include <sqlite3.h>

int open_db_connection(void);
int close_db_connection(void);
sqlite3* getDb();
