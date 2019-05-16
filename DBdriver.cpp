#include "sqlite3.h"
#include <iostream>
#include <string> 
#include "DBdriver.h"

using namespace std;





typedef int (*sqlite3_callback)(
	void*,    /* Data provided in the 4th argument of sqlite3_exec() */
	int,      /* The number of columns in row */
	char**,   /* An array of strings representing fields in the row */
	char**    /* An array of strings representing column names */
	);

string bigArray[100];
int bigSize;


//print sql recoreds
 int DBdriver::callback(void* data, int argc, char** argv, char** azColName)  {

	int i;
	fprintf(stderr, "Record: ", (const char*)data);

	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}

	printf("\n");
	return 0;
}

 void DBdriver::convertValues(int size ,string columnArray[], string valueArray[]) {
	 bigSize = size;
	 for (int i = 0; i < size; i++) {
		 bigArray[i] = valueArray[i];
	 }
 }

 int DBdriver::valueGet(void* data, int size, char** argv, char** azColName) {
	 bigSize = size;
	 for (int i = 0; i < size; i++) {
		 bigArray[i] = argv[i];
	 }
	 return 0;
 }



 //create main table intervals for db
 void DBdriver::createTable(string dbName ,string table, int size , string colName[], string typeVal[]) {
	
	 sqlite3* DB;

	 string ben = " ";
	 
	 for (int i = 0; i < size; i++) {
		
		string ex = colName[i] + " " + typeVal[i] + " NOT NULL";
		if (i != size-1)
		ben = ben + ex + " , ";
		else
		ben = ben + ex;
	 }
	 ben = ben + ");";
	 //cout << ben << endl;

	 string sql = "CREATE TABLE " + table + " (" + ben;

	 int exit = 0;
	 exit = sqlite3_open(dbName.c_str(), &DB);
	 char* messaggeError;
	 exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

	 if (exit != SQLITE_OK) {
		 std::cerr << "Error Create Table" << std::endl;
		 sqlite3_free(messaggeError);
	 }
	 else
		 std::cout << "Table created Successfully" << std::endl;
	 sqlite3_close(DB);
	 
}

 void  DBdriver::fillTable(string dbName, string table, string arrayList[100][20], int col, int valSet, string colName[]) {
	sqlite3* DB;
	char* messaggeError;
	int exit = sqlite3_open(dbName.c_str(), &DB);
	

			string ben = " ";

			for (int i = 0; i < col; i++) {

				string ex = colName[i] + " " ;
				if (i != col-1)
					ben = ben + ex + " , ";
				else
					ben = ben + ex;
			}
			ben = ben + ") VALUES ";
			//cout << ben << endl;
			int val;
			for (int j = 0; j < valSet; j++) {
				ben = ben + " ( ";
				for (int i = 0; i < col; i++) {

					string ex = arrayList[j][i] + " ";
					if (i != col-1)
						ben = ben + ex + " , ";
					else
						ben = ben + ex;
				}
				if (j != valSet-1)
					ben = ben + ") , ";
				else
					ben = ben + ") ;";
			}
			//cout << ben << endl;
		

			string sql = "INSERT OR REPLACE INTO " + table + " (" + ben;

			//cout << "FINAL: " << sql << endl;

			exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);
			if (exit != SQLITE_OK) {
				std::cerr << "Error Insert" << std::endl;
				sqlite3_free(messaggeError);
			}
			else
				std::cout << "Records created Successfully!" << std::endl;

		
	
	
	
	
	sqlite3_close(DB);
	
}

void DBdriver::deleteRecord(string dbName , string table,  string whereClause, string clauseVal/*,sqlite3* DB*/) {
	
	sqlite3* DB;
	char* messaggeError;
	
	int exit = sqlite3_open(dbName.c_str(), &DB);
	string query = "SELECT * FROM " +table+";";

	string sql = "DELETE FROM "+ table +" WHERE " + whereClause + " = " + clauseVal +" ;";
	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);
	if (exit != SQLITE_OK) {
		std::cerr << "Error DELETE" << std::endl;
		sqlite3_free(messaggeError);
	}
	else
		std::cout << "Record deleted Successfully!" << std::endl;

	//cout << "STATE OF TABLE AFTER DELETE OF ELEMENT" << endl;
	//sqlite3_exec(DB, query.c_str(), callback, NULL, NULL);

	sqlite3_close(DB);
}

// dont know how to get records into int,double arrays?
void DBdriver::getRecord(string dbName, string table, string columnName, string dataArray[]) {
	sqlite3* DB;
	char* messaggeError;
	int exit = sqlite3_open(dbName.c_str(), &DB);
	string query = "SELECT DISTINCT " + columnName + " FROM " + table + " ;";
	sqlite3_exec(DB, query.c_str(), valueGet, NULL, NULL);
	cout << " --Print saved array-- " << endl;
	for (int i = 0; i < bigSize; i++) {
		dataArray[i] = bigArray[i];
		cout << dataArray[i] << endl;
	}
}
		


void DBdriver::updateRecord(string dbName, string table, string whereCause,string id,string column, string value) {
	sqlite3* DB;
	char* messaggeError;
	int exit = sqlite3_open(dbName.c_str(), &DB);
	string query = "SELECT * FROM "+table+" ;";

	//cout << "STATE OF TABLE BEFORE UPDATE" << endl;
	//sqlite3_exec(DB, query.c_str(), callback, NULL, NULL);
	

	string sql = "UPDATE "+table+" SET " + column + " = " + value + " WHERE "+whereCause+" = " + id + " ;";
	

	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);
	if (exit != SQLITE_OK) {
		std::cerr << "Error Insert" << std::endl;
		sqlite3_free(messaggeError);
	}
	else
		std::cout << "Records created Successfully!" << std::endl;



	
}

void DBdriver::showRecords(string dbName, string table) {
	sqlite3* DB;
	char* messaggeError;
	int exit = sqlite3_open(dbName.c_str(), &DB);
	string query = "SELECT * FROM " + table + " ;";

	cout << "--TABLE RECORDS--" << endl;
	sqlite3_exec(DB, query.c_str(), callback, NULL, NULL);
	cout << "--END OF TABLE--" << endl;
}