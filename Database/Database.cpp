#include "Database.h"

Database::Database(string dbfile) 
{
	int rc = 1; // return code
	stmt = 0;
	verbosity = 2;
	logstream = stdout;
	rc = sqlite3_open(dbfile.c_str(), &database);
	// anything other than 0 returned is an error code
	if(rc != SQLITE_OK) {
		log("There was an error loading the database: \n" + (string)sqlite3_errmsg(database) + "\n", CUSTOM_ERROR);
	} else {
		log("Database loaded successfully\n", OTHER);
	}
}

Database::~Database() 
{
	if(stmt)
		sqlite3_finalize(stmt);
	if(database)
		sqlite3_close(database);
}

void Database::log(string message, int type)
{
	if(type == STANDARD_ERROR && verbosity >= 1) {
		fprintf(logstream, "***\nError in database statement:\nStatement: %s\nError: %s\n", message.c_str(), sqlite3_errmsg(database));
	}
	if(type == CUSTOM_ERROR && verbosity >= 1) {
		fprintf(logstream, message.c_str());
	}
	if(type == WARNING && verbosity >= 2) {
		fprintf(logstream, "***\nWarning for database statement:\nStatement: %s\n", message.c_str());
	}
	if(type == OTHER && verbosity >= 3) {
		fprintf(logstream, message.c_str());
	}
}


int Database::dml(string sql) 
{
	int rc = 1;

	rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, 0);
	if(rc != SQLITE_OK) {
		log(sql, STANDARD_ERROR);
	} else {
		log("DML statement accepted\n", OTHER);
		rc = sqlite3_step(stmt);
		if(rc != SQLITE_DONE) {
			log(sql, STANDARD_ERROR);
		} else {
			rc = sqlite3_finalize(stmt);
			stmt = 0;
			if(rc != SQLITE_OK) {
				log(sql, STANDARD_ERROR);
			}
		}
	}
	return rc;
}



int Database::query(string sql)
{
	// close stmt if it's already open
	if(stmt) {
		sqlite3_finalize(stmt);
		stmt = 0;
	}
	int rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, 0);
	if(rc) {
		log(sql, 1);
	} else {
		log("Query statement accepted\n", OTHER);
//		columnCount = sqlite3_column_count(stmt);
//		curColumn = 0;
	}
	return rc;
}
/* // for retrieving a single column rather than a whole row
char* Database::getNextResult()
{
	int rowExists = SQLITE_ROW;

	if(curColumn == columnCount) {
		curColumn = 0;
	}
	if(curColumn == 0) {
		rowExists = sqlite3_step(stmt);
	}
	if(rowExists == SQLITE_ROW) {
		result = (char*) sqlite3_column_text(stmt, curColumn);
		curColumn++;
	} else {
		result = 0;
	}
	return result;
}
*/
string Database::getNextRow(string delim)
{
	string resultStr;
	resultStr.clear();
	int columns = sqlite3_column_count(stmt); // TODO prepared?
	int col = 0;
	if(sqlite3_step(stmt) == SQLITE_ROW) {
		for(col = 0; col < columns; col++) {
			// add the next column and a delimiter
			resultStr.append(((char*) sqlite3_column_text(stmt, col)) + delim);
		}
		resultStr.erase(resultStr.length()-delim.length(), resultStr.length()); // remove the last delimiter
	} else {
	//	printf("There was an error getting the next row:\nError: %s\n", sqlite3_errmsg(database));
		return "";
	}
	return resultStr;
}

int Database::importTable(string filename)
{
	int rc = 1;

	ifstream tFile(filename.c_str());
	// remove the file extension
	string tableName = filename.erase(filename.length()-4, filename.length()-1);
	// remove the path
	tableName = tableName.erase(0, tableName.find('/')+1);
	string sql;
	string line;
	string strBuilder;
	int columns = 0;
	int curCol = 0;


	// the first line is the table creation line with column names
	getline(tFile, line, '\n');
	sql = "CREATE TABLE " + tableName + "(" + line + ");";
	rc = dml(sql);
	if(rc != SQLITE_OK) {
		log("There was an error during import of table " + filename + ":\nStatement: " + sql + "\nError: " + sqlite3_errmsg(database) + "\n", CUSTOM_ERROR);
		return rc;
	}
/*	strBuilder = line;
	while(strBuilder.find(',') != -1) {
		strBuilder = strBuilder.erase(0, strBuilder.find(',')+1);
		columns++;
	}
	while(!tFile.eof()) {
		strBuilder.clear();
		for(curCol = 0; curCol <= columns; curCol++) {
			getline(tFile, line, ',');
			strBuilder.append("'" + line + "',");
		}
		strBuilder.erase(strBuilder.length()-1, strBuilder.length());
		sql = "INSERT INTO " + tableName + " VALUES(" + strBuilder + ");";
		printf("%s\n", sql.c_str());
		rc = dml(sql.c_str());
	//	printf("%s\n", line.c_str());
	}
*/
	while(getline(tFile, line, '\n')) {
		strBuilder.clear();
		int found;
		int prevPos = 0;
		printf("%s", line.c_str());
		while((found = line.find(',', prevPos)) != line.npos) {
			strBuilder.append("'" + line.substr(prevPos, found-prevPos) + "',");
			prevPos = found+1;
		//	printf("prevPos: %d\n", prevPos);
		}
		// add the last column
		strBuilder.append("'" + line.substr(prevPos, line.npos) + "'");
		sql = "INSERT INTO " + tableName + " VALUES(" + strBuilder + ");";
		printf("%s\n", sql.c_str());
		rc = dml(sql);
		if(rc != SQLITE_OK) {
			log("There was an error during import of table " + filename + ":\nStatement: " + sql + "\nError: " + sqlite3_errmsg(database) + "\n", CUSTOM_ERROR);
			return rc;
		}
	}

	return rc;
}

int Database::loadOrSave(sqlite3 *pInMemory, string zFilename, bool isSave)
{
  int rc;                   /* Function return code */
  sqlite3 *pFile;           /* Database connection opened on zFilename */
  sqlite3_backup *pBackup;  /* Backup object used to copy data */
  sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */
  sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */

  /* Open the database file identified by zFilename. Exit early if this fails
  ** for any reason. */
  rc = sqlite3_open(zFilename.c_str(), &pFile);
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