//c++ -o fit_adc fit_adc.cpp `root-config --cflags --glibs`

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector> 

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TVectorD.h"
#include "TH1D.h"
#include "TH2D.h"

using namespace std;


#define CONVERSIONE 1		//conversione ADC
#define CONVERSIONE_TDC 1	
#define CHANN_ADC0 2
#define CHANN_ADC1 3
#define CHANN_TDC  1

#define MIN 100
#define MAX 1000

bool leggi_dati(vector<double> & ampiezzas1, vector<double> & ampiezzas2, vector<double> & tempi, int argc, char** argv){
	if(argc < 2){
		cout << "Digita il nome del file di dati" << endl;
		return false;
	}
	double scal1, scal2, scal3, scal4, scal5, scal6, scal7, scal8;
	ifstream Infile;
	Infile.open(argv[1], fstream::in);
	if (Infile.good () == false){
		cout << "Errore nell'apertura dei file" << endl;
		return false;
	}
	cout << "Leggo file di dati " << argv[1] << endl;
	while(true){
		//Infile >> scal1 >> scal2 >> scal3 >> scal4 >> scal5 >> scal6 >> scal7 >> scal8;
		Infile >> scal1 >> scal2 >> scal3;
		switch(CHANN_ADC0){
			case 1: 
				ampiezzas1.push_back(scal1* CONVERSIONE);
				break;
			case 2: 
				ampiezzas1.push_back(scal2* CONVERSIONE);
				break;
			case 3: 
				ampiezzas1.push_back(scal3* CONVERSIONE);
				break;
			case 4: 
				ampiezzas1.push_back(scal4* CONVERSIONE);
				break;
			case 5: 
				ampiezzas1.push_back(scal5* CONVERSIONE);
				break;
			case 6: 
				ampiezzas1.push_back(scal6* CONVERSIONE);
				break;
			case 7: 
				ampiezzas1.push_back(scal7* CONVERSIONE);
				break;
			case 8: 
				ampiezzas1.push_back(scal8* CONVERSIONE);
				break;	
			default:
				cout << "\nLa colonna nel define iniziale di s1 e' errata" << endl;
				return 1;
				break;
		}
		switch(CHANN_ADC1){
			case 1: 
				ampiezzas2.push_back(scal1* CONVERSIONE);
				break;
			case 2: 
				ampiezzas2.push_back(scal2* CONVERSIONE);
				break;
			case 3: 
				ampiezzas2.push_back(scal3* CONVERSIONE);
				break;
			case 4: 
				ampiezzas2.push_back(scal4* CONVERSIONE);
				break;
			case 5: 
				ampiezzas2.push_back(scal5* CONVERSIONE);
				break;
			case 6: 
				ampiezzas2.push_back(scal6* CONVERSIONE);
				break;
			case 7: 
				ampiezzas2.push_back(scal7* CONVERSIONE);
				break;
			case 8: 
				ampiezzas2.push_back(scal8* CONVERSIONE);
				break;	
			default:
				cout << "\nLa colonna nel define iniziale di s2 e' errata" << endl;
				return 1;
				break;
		}
		switch(CHANN_TDC){
			case 1: 
				tempi.push_back(scal1* CONVERSIONE_TDC);
				break;
			case 2: 
				tempi.push_back(scal2* CONVERSIONE_TDC);
				break;
			case 3: 
				tempi.push_back(scal3* CONVERSIONE_TDC);
				break;
			case 4: 
				tempi.push_back(scal4* CONVERSIONE_TDC);
				break;
			case 5: 
				tempi.push_back(scal5* CONVERSIONE_TDC);
				break;
			case 6: 
				tempi.push_back(scal6* CONVERSIONE_TDC);
				break;
			case 7: 
				tempi.push_back(scal7* CONVERSIONE_TDC);
				break;
			case 8: 
				tempi.push_back(scal8* CONVERSIONE_TDC);
				break;	
			default:
				cout << "\nLa colonna nel define iniziale di s1 e' errata" << endl;
				return 1;
				break;
		}
		if (Infile.eof() == true) break;
	}
	Infile.close();
	cout << "Numero dati: " << ampiezzas1.size() << endl;
	return true;
}

double myFun(double *x, double *par){		
	double A = par[0];		//tau
	double B = par[1];		//Vth
	double C = par[2];		//offset
	double D = x[0];
	return A*log(D/(D-B))+C;
}

int main(int argc, char *argv[]){
	vector<double> ampiezzas1;
	vector<double> ampiezzas2;
	vector<double> tempi;

	if( !(leggi_dati(ampiezzas1, ampiezzas2, tempi, argc, argv)) ) return 1;

	TApplication myApp("myApp",NULL,NULL);
	TCanvas* c_1 = new TCanvas("c_1","c_1",0,0,700,500);
	gStyle -> SetOptFit(1111);

	TF1 * fun_adc0 = new TF1("fun_adc0",myFun, MIN,MAX,3);
	fun_adc0 -> SetLineColor(kRed);
	TF1 * fun_adc1 = new TF1("fun_adc1",myFun, MIN,MAX,3);
	fun_adc1 -> SetLineColor(kBlue);

	TGraphErrors * gr_0 = new TGraphErrors;
	TGraphErrors * gr_1 = new TGraphErrors;

	gr_0 -> GetXaxis() -> SetTitle("canali ADC");
	gr_0 -> GetYaxis() -> SetTitle("canali TDC");
	gr_0 -> SetMarkerStyle(20);
	gr_0 -> SetMarkerSize(0.5);
	gr_0 -> SetMarkerColor(kRed);

	gr_1 -> GetXaxis() -> SetTitle("canali ADC");
	gr_1 -> GetYaxis() -> SetTitle("canali TDC");
	gr_1 -> SetMarkerStyle(20);
	gr_1 -> SetMarkerSize(0.5);
	gr_1 -> SetMarkerColor(kBlue);

	for(int i = 0; i < tempi.size(); i++){
		double x1 = ampiezzas1[i];
		double x2 = ampiezzas2[i];
		double y = tempi[i];
		int n = gr_0 -> GetN();

		gr_0 -> SetPoint(n, x1, y);
		gr_0 -> SetPointError(n, 1, 1);
		gr_1 -> SetPoint(n, x2, y);
		gr_1 -> SetPointError(n, 1, 1);
	}

	fun_adc0 -> SetParName(0, "Tau");
	fun_adc0 -> SetParName(1, "Vth");
	fun_adc0 -> SetParName(2, "Off");

	fun_adc1 -> SetParName(0, "Tau");
	fun_adc1 -> SetParName(1, "Vth");
	fun_adc1 -> SetParName(2, "Off");

	fun_adc0 -> SetParameter(0, -7);
	fun_adc0 -> SetParameter(1, -9000);
	fun_adc0 -> SetParameter(2, 260);

	fun_adc1 -> SetParameter(0, 300);
	fun_adc1 -> SetParameter(1, -35);
	fun_adc1 -> SetParameter(2, 320);

	gr_0 -> Fit("fun_adc0", "R");
	gr_1 -> Fit("fun_adc1", "R");

	c_1->cd();

	gr_0 -> GetXaxis() -> SetRangeUser(0,1000);
	gr_1 -> GetXaxis() -> SetRangeUser(0,1000);

	gr_0 -> GetXaxis() -> SetLimits(0,1000);
	gr_1 -> GetXaxis() -> SetLimits(0,1000);

	gr_0 -> GetHistogram() -> SetMaximum(500);
	gr_0 -> GetHistogram() -> SetMinimum(0);
	gr_1 -> GetHistogram() -> SetMaximum(500);
	gr_1 -> GetHistogram() -> SetMinimum(0);

	gr_0 -> Draw("AP");
	gr_1 -> Draw("P SAME");

	c_1 -> Modified();
	c_1 -> Update();


	//correzione

	TCanvas* c_2 = new TCanvas("c_2","c_2",0,0,700,500);
	TGraphErrors * gr_2 = new TGraphErrors;
	TGraphErrors * gr_3 = new TGraphErrors;

	for(int i = 0; i < tempi.size(); i++){
		double x1 = ampiezzas1[i];
		double x2 = ampiezzas2[i];
		double adc0 = fun_adc0 -> Eval(x1) - fun_adc0 -> GetParameter(2);
		double adc1 = fun_adc1 -> Eval(x2) - fun_adc1 -> GetParameter(2);

		double y = tempi[i] - adc0 - adc1;
		int n = gr_2 -> GetN();

		gr_2 -> SetPoint(n, x1, y);
		gr_2 -> SetPointError(n, 1, 1);
		gr_3 -> SetPoint(n, x2, y);
		gr_3 -> SetPointError(n, 1, 1);
	}

	c_2->cd();

	gr_2 -> GetXaxis() -> SetRangeUser(0,1000);
	gr_3 -> GetXaxis() -> SetRangeUser(0,1000);

	gr_2 -> GetXaxis() -> SetLimits(0,1000);
	gr_3 -> GetXaxis() -> SetLimits(0,1000);

	gr_2 -> GetHistogram() -> SetMaximum(500);
	gr_2 -> GetHistogram() -> SetMinimum(0);
	gr_3 -> GetHistogram() -> SetMaximum(500);
	gr_3 -> GetHistogram() -> SetMinimum(0);

	gr_2 -> Draw("AP");
	gr_3 -> Draw("P SAME");

	c_2 -> Modified();
	c_2 -> Update();

	myApp.Run();
	return 0;
}
