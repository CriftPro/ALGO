#pragma once
#include <string> 

using std::string;

class DBdriver {
	
	

public:
	void fillTable(string , string , string data[100][20], int col , int valSet , string colName[] ); // fill ALL table with value
	void createTable(string dbname, string table, int size ,string colName[], string typeVal[]);
	void deleteRecord(string , string table, string whereClause, string clauseVal);
	void getRecord(string dbName, string table, string columnName, string data[]);
	void updateRecord(string dbname, string table , string where, string id, string column, string value); // fill specific values
	void showRecords(string dbname, string table);

	static int callback(void* data, int argc, char** argv, char** azColName);
	static int valueGet(void* data, int argc, char** argv, char** azColName);

	void convertValues(int, string columnArray[], string arrayValue[]);
	
private:
	string dbName;
	int dataArray[10];
	double energyArray[10];

};