
#include "sqlite3.h"
#include <iostream>
#include <string> 
#include "DBdriver.h"

using namespace std;


//string dbname, string table , string where, string id, string column, string value
string columnName[13] = {
		"ID",
		"YEAR",
		"MONTH",
		"DAY",
		"PRIORITY",
		"START",
		"END",
		"DURATION",
		"MAXBRIG",
		"MINBRIG",
		"THEORETICAL",
		"PRACTICAL",
		"COEFFICENT" };
string recordValue[13] = {
	"PRIMARY KEY",
	"INT",
	"INT",
	"INT",
	"INT", // PRIOR
	"INT",
	"INT",
	"INT",
	"INT",
	"INT",//mINBR
	"REAL",
	"REAL",
	"REAL",
};


int nomain() {
	string dbname = "house7.db";
	string table = "INTERVALS";
	sqlite3* DB;
	DBdriver db;
	string data[10];

	string data_to_fill[100][20];
	int col = 13;
	int var = 3;
	for (int i = 0; i < var; i++) {
		data_to_fill[i][0] = to_string(i);
		for (int j = 1; j < col; j++) {
			data_to_fill[i][j] = "1";
		}
	}
	
	db.createTable(dbname,table,col,columnName,recordValue);
	db.fillTable(dbname,table,data_to_fill, col , var, columnName);
	db.showRecords(dbname, table);
	db.deleteRecord(dbname,table,"ID","0");
	db.updateRecord(dbname, table, "ID", "1", "DURATION", "1000");
	db.getRecord(dbname, table, "DURATION", data);

	cout << "<<Table getted data>>" << endl;
	for (int i = 0; i < size(data); i++) {
		cout << data[i] << endl;
		
	}

	return 0;
} //sviestuvo // //diena // //istorinis duomenis // 
//type sviestuvo tokiais const 
//type sviestuvos 

