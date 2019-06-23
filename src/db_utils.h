#include <sqlite3.h>

#define DATABASE_FILE_NAME "/var/data/test.db"

int open_db_connection(void);
int close_db_connection(void);
sqlite3* getDb();
