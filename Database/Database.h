#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <vector>
#include <string>
#include "sqlite3.h"

using namespace std;

class Database {
private:
	// primary database connection
	sqlite3 *database;

public:
	// construct using the provided filename as database connection
	Database(const char* dbfile);
	~Database();

	// Non-query statements (not needing to return anything)
//	int dml(const char* sql);

	// select statements
	vector<vector<string>> query(const char* sql);

	// load or save a db from/to a file
	int loadOrSave(sqlite3 *pInMemory, const char *zFilename, bool isSave);
};

#endif