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
	sqlite3 *database;
	// single statement that could be open for querying. gets closed on requery and deconstruct
	sqlite3_stmt* stmt;
	// string result when getNextResult() is called
//	char* result;
	
	// vars which keep track of which column was returned last
//	int curColumn;
//	int columnCount;

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