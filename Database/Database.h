#include <stdio.h>
#include "sqlite3.h"

class Database {
private:
	// primary database connection
	sqlite3 *database;

public:
	// construct with a null database
	Database();
	// construct using the provided filename as database connection
	Database(char* dbfile);

	// load or save a db from/to a file
	int loadOrSave(sqlite3 *pInMemory, const char *zFilename, bool isSave);
};