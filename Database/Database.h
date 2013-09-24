#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <fstream>
#include <string>
#include "sqlite3.h"

using namespace std;

class Database {
private:
	// primary database connection
	sqlite3* database;
	// single statement that could be open for querying. gets closed on requery and deconstruct
	sqlite3_stmt* stmt;
	// log verbosity: 0 = none, 1 = errors only, 2 = errors and warnings (default), 3 = everything
	int verbosity;
	FILE* logstream;
	// string result when getNextResult() is called
//	char* result;
	
	// vars which keep track of which column was returned last
//	int curColumn;
//	int columnCount;

	enum MSGTYPE {STANDARD_ERROR, CUSTOM_ERROR, WARNING, OTHER};

	// log to print to (default is stdout), type is: 1 error, 2 warning, 3 other (matches verbosity)
	void log(string message, int type);

public:
	// construct using the provided filename as database connection
	Database(string dbfile);
	~Database();

	// Non-query statements (not needing to return anything)
	int dml(string sql);

	// select statements
	int query(string sql);

	// retrieve a single result from the last query
//	char* getNextResult();
	string getNextRow(string delim = "|");

	// import a table from a CSV file; returns 0 on success
	int importTable(string filename);

	// load or save a db from/to a file
	int loadOrSave(sqlite3 *pInMemory, string zFilename, bool isSave);
};

#endif