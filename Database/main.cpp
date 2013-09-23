#include <stdio.h>
#include "Database.h"

int main() {
	Database* db = new Database("test.db");
	char name[100];
	int hp = 0;
	string result;

	db->dml("drop table monster;");
	db->importTable("tables/monster.csv");
	db->query("select * from monster;");
	for(result = db->getNextRow(); !result.empty(); result = db->getNextRow()) {
		printf("%s\n", result.c_str());
	}

	/* // store values into typed variables
	if(result)
		strcpy(name, result);
	else
		printf("Name did not exist\n");
	result = db->getNextResult();
	if(result)
		sscanf(result, "%d", &hp);
	else
		printf("hp did not exist\n");
	printf("%s | %d", name, hp);
	*/
/*	db->dml("create table monster(name varchar2(100), hp number(20));");
	db->dml("insert into monster values('Goblin', 150)");
	db->dml("insert into monster values('Dragon', 2000)");
	db->query("select * from monster;");
	while(result = db->getNextResult()) {
		printf("%s", result);
	}
	db->query("select * from monster;");
	result = db->getNextResult();
	strcpy(name, result);
	result = db->getNextResult();
	sscanf(result, "%d", &hp);
	printf("%s | %d", name, hp);
*/
	
	scanf("%*c");
	return 0;
}