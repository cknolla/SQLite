#include "Database.h"

Database::Database(const char* dbfile) 
{
	int rc = 1; // return code
	rc = sqlite3_open(dbfile, &database);
//	rc = loadOrSave(db, dbfile, false);
	// anything other than 0 returned is an error code
	if(rc) {
		printf("There was an error loading the database: \n%s\n", sqlite3_errmsg(database));
	} else {
		printf("Database loaded successfully\n");
	}
}

Database::~Database() 
{
	if(database)
		sqlite3_close(database);
}

/*
int Database::dml(const char* sql) 
{
	sqlite3_stmt* statement;
	int rc = 1;

	rc = sqlite3_prepare_v2(database, sql, -1, &statement, 0);
	if(rc) {
		printf("***\nThere was a DML statement error:\nStatement: %s\nError: %s\n***\n", sql, sqlite3_errmsg(database));
	} else {
		printf("DML statement accepted\n");
	}

	rc = sqlite3_finalize(statement);
	if(rc) {
		printf("There was an error finalizing a DML statement:\nStatement: %s\nError: %s\n", sql, sqlite3_errmsg(database));
	}
	return rc;
}
*/
vector<vector<string>> Database::query(const char* sql)
{
	sqlite3_stmt* statement;
	vector<vector<string>> results;
	int rc = 1;

	rc = sqlite3_prepare_v2(database, sql, -1, &statement, 0);
	if(rc) {
		printf("There was a query statement error:\nStatement: %s\nError: %s\n", sql, sqlite3_errmsg(database));
	} else {
		printf("Query statement accepted\n");
		int cols = sqlite3_column_count(statement);
		int result = 0;
		while((result = sqlite3_step(statement)) == SQLITE_ROW) {
			vector<string> values;
			for(int col = 0; col < cols; col++) {
				values.push_back((char*) sqlite3_column_text(statement, col));
			//	printf("\n%s\n", *results);
			}
			results.push_back(values);
		}
		rc = sqlite3_finalize(statement);
	}
	return results;
}

int Database::loadOrSave(sqlite3 *pInMemory, const char *zFilename, bool isSave)
{
  int rc;                   /* Function return code */
  sqlite3 *pFile;           /* Database connection opened on zFilename */
  sqlite3_backup *pBackup;  /* Backup object used to copy data */
  sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */
  sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */

  /* Open the database file identified by zFilename. Exit early if this fails
  ** for any reason. */
  rc = sqlite3_open(zFilename, &pFile);
  if( rc==SQLITE_OK ){

    /* If this is a 'load' operation (isSave==0), then data is copied
    ** from the database file just opened to database pInMemory. 
    ** Otherwise, if this is a 'save' operation (isSave==1), then data
    ** is copied from pInMemory to pFile.  Set the variables pFrom and
    ** pTo accordingly. */
    pFrom = (isSave ? pInMemory : pFile);
    pTo   = (isSave ? pFile     : pInMemory);

    /* Set up the backup procedure to copy from the "main" database of 
    ** connection pFile to the main database of connection pInMemory.
    ** If something goes wrong, pBackup will be set to NULL and an error
    ** code and  message left in connection pTo.
    **
    ** If the backup object is successfully created, call backup_step()
    ** to copy data from pFile to pInMemory. Then call backup_finish()
    ** to release resources associated with the pBackup object.  If an
    ** error occurred, then  an error code and message will be left in
    ** connection pTo. If no error occurred, then the error code belonging
    ** to pTo is set to SQLITE_OK.
    */
    pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
    if( pBackup ){
      (void)sqlite3_backup_step(pBackup, -1);
      (void)sqlite3_backup_finish(pBackup);
    }
    rc = sqlite3_errcode(pTo);
  }

  /* Close the database connection opened on database file zFilename
  ** and return the result of this function. */
  (void)sqlite3_close(pFile);
  return rc;
}