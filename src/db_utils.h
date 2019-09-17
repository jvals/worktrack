#include <sqlite3.h>

void set_datebase_path(char*);
int open_db_connection(void);
int close_db_connection(void);
sqlite3* getDb();
