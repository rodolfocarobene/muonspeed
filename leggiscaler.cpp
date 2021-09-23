//c++ -o leggiscaler leggiscaler.cpp `root-config --cflags --glibs`
// ./leggiscaler /mnt/c/Users/Rodolfo/Desktop/calibrazione/3/ 

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector> 

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphErrors.h"

using namespace std;

#define CHANNEL_SING0 	1			//s1
#define CHANNEL_SING1 	2			//s2
#define CHANNEL_DOPP	3

#define DEBUG false

bool leggi_datiSCA(vector<int> & vec,  int channel, char* myFile){
	int scal1, scal2, scal3, scal4, scal5, scal6, scal7, scal8, scal9, scal10, scal11, scal12;
	ifstream Infile;
	Infile.open(myFile, fstream::in);
	if (Infile.good () == false){
		cout << "Errore nell'apertura dei file " << myFile << endl;
		return false;
	}
	cout << "Leggo file di dati " << myFile << endl;
	while(true){
		Infile >> scal1 >> scal2 >> scal3 >> scal4 >> scal5 >> scal6 >> scal7 >> scal8 >> scal9 >> scal10 >> scal11 >> scal12;
		switch(channel){
			case 1: 
				vec.push_back(scal1);
				break;
			case 2: 
				vec.push_back(scal2);
				break;
			case 3: 
				vec.push_back(scal3);
				break;
			case 4: 
				vec.push_back(scal4);
				break;
			case 5: 
				vec.push_back(scal5);
				break;
			case 6: 
				vec.push_back(scal6);
				break;
			case 7: 
				vec.push_back(scal7);
				break;
			case 8: 
				vec.push_back(scal8);
				break;	
			case 9: 
				vec.push_back(scal9);
				break;
			case 10: 
				vec.push_back(scal10);
				break;
			case 11: 
				vec.push_back(scal11);
				break;
			case 12: 
				vec.push_back(scal12);
				break;	
			default:
				cout << "\nLa colonna nel define e' errata" << endl;
				return 1;
				break;
		}
		if (Infile.eof() == true) break;
	}
	Infile.close();
	cout << "Numero dati: " << vec.size() << endl;
	return true;
}

double getMean(vector<int> & vec){
	double sum = 0;
	for(int i = 0; i < vec.size(); i++){
		sum = sum + (double)vec[i];
	}
	return sum / vec.size();
}

double getDev(vector<int> & vec, double mean){
	double sum = 0;
	for(int i = 0; i < vec.size(); i++){
		sum = sum + pow((double)vec[i] - mean,2);
	}
	int N = vec.size() - 1; 
	sum = sqrt(sum / N);
	sum = sum / sqrt(N+1);
	return sum;
}

int main(int argc, char *argv[]){
																													if (DEBUG == true)	cout << __LINE__ << endl;
	if(argc < 2) return 1;
	if(strcmp(argv[1],"help")==0){
		cout << "----------------------------------" << endl;
		cout << "Scopo:\n"
			 << "\tIlprogramma legge e grafica i dati dello scaler con due canali di singola e un canale di doppia\n" << endl;
		cout << "Utilizzo:\n"
			 << "\t ./leggiscaler file.txt"
			 << "\t Da definire i canali utilizzati\n" << endl;
		cout << "----------------------------------" << endl;
		return 1;
	}

	vector<int> singola0;
	vector<int> singola1;
	vector<int> doppie;

	vector<int> diff_singola0;
	vector<int> diff_singola1;
	vector<int> diff_doppie;

	leggi_datiSCA(singola0,CHANNEL_SING0,argv[1]);
	leggi_datiSCA(singola1,CHANNEL_SING1,argv[1]);
	leggi_datiSCA(doppie,CHANNEL_DOPP,argv[1]);

	cout << "Lettura completata" << endl;


	//-----------------------grafici-----------------------

	TApplication myApp("myApp",NULL,NULL);
	TCanvas* c_sca = new TCanvas("c_sca","c_sca",0,0,700,500);		
	TGraphErrors * gr_sca = new TGraphErrors;

	gr_sca -> GetXaxis() -> SetTitle("Minuti passati");
	gr_sca -> GetYaxis() -> SetTitle("Conteggi al minuto");
	gr_sca -> SetTitle("Conteggi doppie");
	gr_sca -> SetMarkerStyle(20);
	gr_sca -> SetMarkerSize(0.5);
	gr_sca -> SetMarkerColor(kRed);

	gr_sca -> SetPoint(gr_sca -> GetN(), 0, 0);

	double yn = 0;

	for(int i = 0; i < doppie.size(); i++){
		double x = i;
		int N = gr_sca -> GetN();
		double y = (doppie[i] - yn);
		yn = doppie[i];
		gr_sca -> SetPoint(N,x,y);
		diff_doppie.push_back(y);
	}
																													if (DEBUG == true)	cout << __LINE__ << endl;

	c_sca -> cd();

	gr_sca -> Draw("AP");

	//singole
	TCanvas* c_scasing = new TCanvas("c_scasing","c_scasing",0,0,700,500);
	TGraphErrors * gr_sca0 = new TGraphErrors;
	TGraphErrors * gr_sca1 = new TGraphErrors;

	gr_sca0 -> GetXaxis() -> SetTitle("Minuti passati");
	gr_sca0 -> GetYaxis() -> SetTitle("Conteggi al minuto");
	gr_sca0 -> SetTitle("Conteggi S1");
	gr_sca0 -> SetMarkerStyle(20);
	gr_sca0 -> SetMarkerSize(0.5);
	gr_sca0 -> SetMarkerColor(kRed);

	gr_sca1 -> GetXaxis() -> SetTitle("Minuti passati");
	gr_sca1 -> GetYaxis() -> SetTitle("Conteggi al minuto");
	gr_sca1 -> SetTitle("Conteggi S2");
	gr_sca1 -> SetMarkerStyle(20);
	gr_sca1 -> SetMarkerSize(0.5);
	gr_sca1 -> SetMarkerColor(kRed);

	gr_sca0 -> SetPoint(gr_sca0 -> GetN(), 0, 0);
	gr_sca1 -> SetPoint(gr_sca1 -> GetN(), 0, 0);

	double yn0 = 0;
	double yn1 = 0;

	for(int i = 0; i < singola0.size(); i++){
		double x = i;
		int N = gr_sca0 -> GetN();
		double y = (singola0[i] - yn0);
		yn0 = singola0[i];
		gr_sca0 -> SetPoint(N,x,y);
		diff_singola0.push_back(y);
	}
																													if (DEBUG == true)	cout << __LINE__ << endl;
	for(int i = 0; i < singola1.size(); i++){
		double x = i;
		int N = gr_sca1 -> GetN();
		double y = (singola1[i] - yn1);
		yn1 = singola1[i];
		gr_sca1 -> SetPoint(N,x,y);
		diff_singola1.push_back(y);
	}

	c_scasing -> Divide(1,2);

	c_scasing -> cd(1);
	gr_sca0 -> Draw("AP");
	c_scasing -> cd(2);
	gr_sca1 -> Draw("AP");

																													if (DEBUG == true)	cout << __LINE__ << endl;

	//-----------------------risultati-----------------------

	cout << endl;
	cout << "Media s1: " << getMean(diff_singola0) << " +- " << getDev(diff_singola0, getMean(diff_singola0)) << endl;
	cout << "Media s2: " << getMean(diff_singola1) << " +- " << getDev(diff_singola1, getMean(diff_singola1)) << endl;
	cout << "Media doppie: " << getMean(diff_doppie) << " +- " << getDev(diff_doppie, getMean(diff_doppie)) << endl;


	myApp.Run();
}