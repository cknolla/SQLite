#include <stdio.h>
#include "Database.h"

int main() {
	Database* db = new Database("test.db");
	char name[100];
	char* pName = name;
//	db->dml("create database
//	db->query("create table names(one varchar2(10));", &pName);
//	db->query("insert into names values('Hello');", &pName);
	printf("\n%s\n", pName);
	db->query("select * from names;");
	printf("\n%s\n", name);
	scanf("%*c");
	return 0;
}