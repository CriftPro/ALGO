// PV_algo.cpp : Defines the entry point for the console application.
//



// This function will be used in case when there is not enough power available for user intended requirements
// Function distibutes power consuption by lowering brighness in low priority intervals until min_brighness reached
// In this example, we need 46 AH to statisfy user's plan
#pragma warning(disable : 4996)



#include <iostream>
#include <iomanip>
#include <fstream>
//#include <cmath>
#include "DBdriver.h"
#include <ctime>

using namespace std;

const double pi = 3.1428571429 ;

ifstream read("Duom.txt");

int max_power_consumption = 60; // in watts
double voltage = 12.25;
int N = 3; //Number of intervals

double energy_left = 0; // Used for tracking how much energy can be used for each time interval

void monthly_PV_output(){

	double Betad = 15; // moduliu pasvirimo kampas su horizontu
double a = 1.67; // modulio ilgis
double b = 0.992; // modulio plotis
double naud = 0.1811; // modulio naudingumo koeficientas
double nuost = 0.05; // galios ir kiti nuostoliai

double n = 6; // moduliu skaicius

double Fid = 54.7; // geografine platuma
double Azimuthd = 0; // modulio kryptis i pasaulio ðalis
double Hh[12] = { 11.9,23.5,66.2,118.7,141.4,153.7,155.1,119.4,83.1,40.1,13.7,6.7 }; // vidutine menesine apðvieta horizantaliam pavirðiui
double Hd[12] = { 9.3,17.6,40.3,62.8,72.7,79.7,93.3,55.5,49.2,22.9,10.7,5.6 }; // vidutine menesine horizonto atspindeta apðvieta
double Md[12] = { 15.00,45.00,74.00,105.00,135.00,166.00,196.00,227.00,258.00,288.00,319.00,349.00 };   // Diena metuose, parenkamos menesio vidurio dienos
double Albedo = 0.35; // pavirðiaus atspindþio koeficientas
double gama = -0.0042; // temperaturos itakos modulio galiai koeficientas
double Temp[12] = { -3.4,-3.7,0.5,7.7,12.8,15.3,18.6,17.3,12.2,7,2.2,-2 }; // vidutine menesine temperatura

double w, Beta, Fi, Azimuth;

double Ddr[12], bbb[12], COStetaz[12], COSteta[12], Rb[12], Kt[12], Rrmatrica[12], skirtumas[12], sandauga[12], finalas[12], sutempe[12], Hbenuost[12], nuostoliai[12], Hviso[12];
w = 0;
Beta = (Betad * pi ) / 180; // modulio pasvirimo kampas su horizontu radianais
Fi = (Fid*pi) / 180; //  geografine platuma radianais
Azimuth = (Azimuthd*pi) / 180; // azimutas radianais
double Rd = (1 + cos(Beta)) / 2; // horizonto atsispindejusios apšvietos santykis tarp  horizantalaus ir kampu beta pakreipto paviršiaus
double Rr = Albedo*(1 - cos(Beta)) / 2; // paviršiaus atsispindejusios apšvietos santykis tarp  horizantalaus ir kampu beta pakreipto paviršiaus


for (int i = 0; i < 12; i++) // skaiciavimai
{
	Ddr[i] = (23.45  * (pi / 180)) * sin((360 * (284 + Md[i])) / 365); // deklinacija radianais
	COStetaz[i] = sin(Fi)*sin(Ddr[i]) + cos(Fi)*cos(Ddr[i])*cos(w); // saules kampas tarp statmens ir horizantalaus paviršiaus
	COSteta[i] = sin(Fi)*((sin(Ddr[i])*cos(Beta)) + (cos(Ddr[i])*cos(Azimuth)*cos(w)*sin(Beta))) + cos(Fi)*(cos(Ddr[i])*cos(w)*cos(Beta) - sin(Ddr[i])*cos(Azimuth)*sin(Beta)) + (cos(Ddr[i])*sin(Azimuth)*sin(w)*sin(Beta)); // saules kampas tarp statmens pasvirusio objekto kampu beta atžvilgiu
	Rb[i] = COStetaz[i] / COSteta[i]; // santykis tarp horizonto apšvietos krentancios horizontaliame paviršiui ir kampu beta paviršiui.
	Kt[i] = 1 - (gama*(25 - Temp[i] - 29.5)); // temperaturos itaka
	Rrmatrica[i] = Rr;
	skirtumas[i] = Hh[i];
	bbb[i] = Rb[i] * skirtumas[i];
	sandauga[i] = Rd * Hd[i];
	finalas[i] = bbb[i] + sandauga[i] + Rrmatrica[i];
	sutempe[i] = finalas[i] * Kt[i];
	Hbenuost[i] = a*b*naud*n*sutempe[i];
	nuostoliai[i] = nuost*Hbenuost[i];

	Hviso[i] = Hbenuost[i] - nuostoliai[i];

	cout << i + 1 << " menesio uzkrovimo prognozes: " << Hviso[i] << endl;

}


};

struct on_array // intervals in which lamp will be on
{
	double on_time; // In minutes
	double off_time; // In minutes
	double min_brighness; // in percent
	double max_brighness; // in percent
	double energy_needed; // how much battery power needed using max_brighness

	double actual_energy_used;
	double energy_need_adj = 1; // cof used to correct energy demand
};

double List[10][10];

struct on_array on_array[20];
struct on_array new_on_array[20];

/********************************************************/
void read_file() // read file for example
{

	int temp = 0;

	while (!read.eof())
	{
		read >> temp;
		temp--; // because of indexes
		read >> on_array[temp].on_time >> on_array[temp].off_time >> on_array[temp].min_brighness >> on_array[temp].max_brighness;
	}

	for (int i = 0; i < N; i++) // calculates energy_needed in on_array
	{
		on_array[i].energy_needed = ((on_array[i].off_time - on_array[i].on_time) / 60) * ((on_array[i].max_brighness / 100) * (max_power_consumption / voltage)); // for calculating time it will work for now (:
																																								   //cout << on_array[i].energy_needed << endl;
	}


}
/********************************************************/

/*
string today_time(string time) {
	std::time_t rawtime;
	std::tm* timeinfo;
	char buffer[80];

	std::time(&rawtime);
	timeinfo = localtime(&rawtime);

	if (time == "%Y") {
		std::strftime(buffer, 80, "%Y", timeinfo);// "%Y-%m-%d-%H-%M-%S"
	}

	if (time == "%m") {
		std::strftime(buffer, 80, "%m", timeinfo);
	}
	if (time == "%d") {
		std::strftime(buffer, 80, "%d", timeinfo);
	}

	std::puts(buffer);
	return buffer;
}
*/

void convert_to_string(string dataArray[50][20]) {
	int col = 13;
	int var = N;
	for (int i = 0; i < var; i++) {
		dataArray[i][0] = to_string(i);
		dataArray[i][1] = "2019";/*today_time("%Y");*/
		dataArray[i][2] = "05";/*today_time("%m");*/
		dataArray[i][3] = "16";/*today_time("%d");*/
		dataArray[i][4] = to_string(i);
		dataArray[i][5] = to_string(on_array[i].on_time);
		dataArray[i][6] = to_string(on_array[i].off_time);
		dataArray[i][7] = to_string(on_array[i].off_time - on_array[i].on_time);
		dataArray[i][8] = to_string(on_array[i].max_brighness);
		dataArray[i][9] = to_string(on_array[i].min_brighness);
		dataArray[i][10] = to_string(on_array[i].energy_needed);
		dataArray[i][11] = to_string(on_array[i].actual_energy_used);
		dataArray[i][12] = to_string(on_array[i].energy_need_adj);
	}
	
	for (int i = 0; i < var; i++) {
		for (int j = 0; j < col; j++) {
			cout << "| "<< i << "| "<< j << " | "<< dataArray[i][j] << endl;
		}
	}
}

void connect_To_Db(string data_to_fill[50][20], int col , int var) {
	DBdriver db;
	

	string dbname = "ALGO_DUOM.db";
	string table = "INTERVALS";
	string data[10];

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
		"REAL",
		"REAL",
		"REAL",
		"REAL",
		"REAL",//mINBR
		"REAL",
		"REAL",
		"REAL",
	};
	
	db.createTable(dbname, table, col, columnName, recordValue);
	db.fillTable(dbname, table, data_to_fill, col, var, columnName);
	db.showRecords(dbname, table);
	//db.deleteRecord(dbname, table, "ID", "0");
	//db.updateRecord(dbname, table, "ID", "1", "DURATION", "1000");
	//db.getRecord(dbname, table, "DURATION", data);



}



/********************************************************/
//new_plan() distrubutes energy by calculating how low brighness can be set until it reaches min_brighness level for time interval N
// Starts from highest priority, where maintaining user desired light level is most important
// Uses some sketchy calculation methods
void new_plan_calculations(double power_available)
{
	energy_left = power_available;

	for (int i = 0; i< N; i++)
	{
		new_on_array[i] = on_array[i]; // we'll just change new max_brighness and energy_needed later on

		new_on_array[i].max_brighness = on_array[i].min_brighness; // set to minimum

		new_on_array[i].energy_needed = (new_on_array[i].max_brighness * on_array[i].energy_needed) / on_array[i].max_brighness; // i dunno if this works

		energy_left = energy_left - new_on_array[i].energy_needed;

	}

	for (int i = 0; i < N && energy_left > 0; i++)
	{

		double temp = on_array[i].energy_needed - new_on_array[i].energy_needed; // energy difference between min and max

		if (energy_left - temp < 0) // not enough energy for max brighness
		{
			new_on_array[i].energy_needed = new_on_array[i].energy_needed + energy_left; // new energy needed is energy usage with min brighness + energy left

			new_on_array[i].max_brighness = (new_on_array[i].energy_needed * on_array[i].max_brighness) / on_array[i].energy_needed; // sets new brighness

			energy_left = 0; //  no energy left, because we used it all
		}
		else
		{
			new_on_array[i].energy_needed = on_array[i].energy_needed; // there is enough energy with max brighness
			new_on_array[i].max_brighness = on_array[i].max_brighness;

			energy_left = energy_left - temp; // sub max and min energy usage difference
		}

	}
}

/********************************************************/

void new_plan()
{
	double energy = 0;

	while (energy < 1)
	{
		energy++;

		double used_energy = 0;

		

		cout << "Iveskite Energijos kieki, kuri reikia paskirstyti(AH): "; cin >> energy; cout << endl;

		new_plan_calculations(energy); // amount of power that can be used in one night, AH

		for (int i = 0; i < N; i++)
		{
			used_energy = used_energy + new_on_array[i].energy_needed;
			cout << new_on_array[i].energy_needed << endl;
		}

		if (used_energy > energy)
		{
			for (int i = 0; i < N; i++)
			{
				on_array[i].max_brighness = on_array[i].min_brighness;
				on_array[i].min_brighness = on_array[i].min_brighness / 2;

			}

			new_plan_calculations(energy);

			used_energy = 0;

			for (int i = 0; i < N; i++)
			{
				used_energy = used_energy + new_on_array[i].energy_needed;
				cout << new_on_array[i].energy_needed << endl;
			}
		}

		cout << "Kiek energijos sunaudota: " << used_energy << endl;

		cout << "Naujas planas:" << endl;

		for (int i = 0; i < N; i++)
		{

			cout << "Intervalo Prioritetas: " << i + 1 << endl;
			cout << "Naujas Apskaiciuotas Ryskumas: " << new_on_array[i].max_brighness << endl;
		}

		system("pause");
	}
}

void calculations_correction()
{
	on_array[0].actual_energy_used = 10;
	on_array[1].actual_energy_used = 15.5;
	on_array[2].actual_energy_used = 22;

	for (int i = 0; i < N; i++)
	{
		if (on_array[i].actual_energy_used / on_array[i].energy_needed != 1)
		{
			on_array[i].energy_need_adj = on_array[i].actual_energy_used / on_array[i].energy_needed;
		}

		on_array[i].energy_needed = on_array[i].energy_needed * on_array[i].energy_need_adj;
	}
}


int main()
	
{
	string data[100][20];

	read_file(); // duomenu skaitymas

	convert_to_string(data);
	
	connect_To_Db(data, 12, N);

	
	
	new_plan(); // naujas planas ()jeigu neuztenka energijos

	convert_to_string(data);

	connect_To_Db(data, 12, N);

	//calculations_correction(); // self learning demo

	//monthly_PV_output(); // menesio PV sugeneruota energija



	
	return 0;
}



