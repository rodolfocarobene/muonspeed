//c++ -o adc_histo adc_histo.cpp `root-config --cflags --glibs`

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
#include "TPaveStats.h"

using namespace std;

#define NBIN 100
#define CONVERSIONE 1		//conversione ADC
#define CONVERSIONE_TDC 1	
#define CHANN_ADC0 2
#define CHANN_ADC1 3
#define CHANN_TDC  1

#define MINS1 10
#define MAXS1 990

#define MINS2 10
#define MAXS2 990

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
	int maxs1 = 0;
	vector<double> ampiezzas2;
	int maxs2 = 0;	
	vector<double> tempi;

	if( !(leggi_dati(ampiezzas1, ampiezzas2, tempi, argc, argv)) ) return 1;

	TApplication myApp("myApp",NULL,NULL);
	TCanvas* c_1 = new TCanvas("c_1","c_1",0,0,700,500);
	TCanvas* c_2 = new TCanvas("c_2","c_2",0,0,700,500);
	gStyle -> SetOptFit(1111);
	gStyle -> SetOptStat(1111);

	for(int i = 0; i < ampiezzas1.size(); i++){
		if (ampiezzas1[i] > maxs1) maxs1 = ampiezzas1[i];
	}
	TH1D* th_1 = new TH1D("ADC S1", "ADC S1", NBIN, 0, maxs1);
	for(int i = 0; i < ampiezzas1.size(); i++){
		if(ampiezzas1[i] != 0 && ampiezzas1[i] < 1000) th_1 -> Fill(ampiezzas1[i]);
	}
	c_1->cd();
	th_1 -> SetFillColor(kCyan);
	th_1 -> Draw();
	cout << "Il massimo di S1 si trova a x = " << th_1 -> GetXaxis() -> GetBinCenter(th_1 -> GetMaximumBin()) << endl;

	for(int i = 0; i < ampiezzas2.size(); i++){
		if (ampiezzas2[i] > maxs2) maxs2 = ampiezzas2[i];
	}
	TH1D* th_2 = new TH1D("ADC S2", "ADC S2", NBIN, 0, maxs2);
	for(int i = 0; i < ampiezzas1.size(); i++){
		if(ampiezzas2[i] != 0 && ampiezzas2[i] < 1000) th_2 -> Fill(ampiezzas2[i]);
	}
	c_2->cd();
	th_2 -> SetFillColor(kCyan);
	th_2 -> Draw();
	cout << "Il massimo di S1 si trova a x = " << th_2 -> GetXaxis() -> GetBinCenter(th_2 -> GetMaximumBin()) << endl;


/*
	TCanvas* c_3 = new TCanvas("c_3","c_3",0,0,700,500);
	TCanvas* c_4 = new TCanvas("c_4","c_4",0,0,700,500);

	TH2D* th_3 = new TH2D("ADC S1-S2", "ADC S21-S", NBIN, 0, maxs1, NBIN, 0, maxs2);
	for(int i = 0; i < ampiezzas1.size(); i++){
		double x = ampiezzas1[i];
		double y = ampiezzas2[i];
		th_3 -> Fill(x,y);
	}
	c_3->cd();
	th_3 -> Draw("surf1 [cutg]");
	c_4->cd();
	th_3 -> Draw("cont4z");
*/

	TCanvas* c_5 = new TCanvas("c_5","c_5",0,0,700,500);
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

	double delta1 = (th_1 -> GetXaxis () -> GetBinCenter(20) - th_1 -> GetXaxis () -> GetBinCenter(19));
	double delta2 = (th_2 -> GetXaxis () -> GetBinCenter(20) - th_2 -> GetXaxis () -> GetBinCenter(19));

	for (int i = 0; i < NBIN; i++){
		double x1 = th_1 -> GetXaxis () -> GetBinCenter(i);
		double x2 = th_2 -> GetXaxis () -> GetBinCenter(i);
		if (x1 > MINS1 && x1 < MAXS1){
			double min = (x1 - delta1)/2;
			double max = (x1 + delta1)/2;
			double mean = 0;
			double std = 0;
			double count = 0;
			for(int i = 0; i < ampiezzas1.size(); i++){
				double x = ampiezzas1[i];
				if (x > min && x < max){
					mean = mean + tempi[i];
					count++;
				}
			}
			mean = mean / count;
			for(int i = 0; i < ampiezzas1.size(); i++){
				double x = ampiezzas1[i];
				if (x > min && x < max){
					if (count > 1) std = std + (x - mean)*(x - mean)/(count - 1);
					else std = 0;
				}
			}
			std = sqrt(std) / sqrt(count);
			double y = mean;
			int n = gr_0 -> GetN();
			gr_0 -> SetPoint(n, x1, y);
			gr_0 -> SetPointError(n, 0, std);

		}

		if (x2 > MINS2 && x2 < MAXS2){
			double min = (x2 - delta2)/2;
			double max = (x2 + delta2)/2;
			double mean = 0;
			double std = 0;
			double count = 0;
			for(int i = 0; i < ampiezzas2.size(); i++){
				double x = ampiezzas2[i];
				if (x > min && x < max){
					mean = mean + tempi[i];
					count++;
				}
			}
			mean = mean / count;
			for(int i = 0; i < ampiezzas2.size(); i++){
				double x = ampiezzas2[i];
				if (x > min && x < max){
					if (count > 1) std = std + (x - mean)*(x - mean)/(count - 1);
					else std = 0;
				}
			}
			std = sqrt(std) / sqrt(count);
			double y = mean;
			int n = gr_1 -> GetN();
			gr_1 -> SetPoint(n, x2, y);
			gr_1 -> SetPointError(n, 0, std);
			
		}
	}

	c_5 -> cd();

	gr_0 -> GetXaxis() -> SetRangeUser(0,1000);
	gr_1 -> GetXaxis() -> SetRangeUser(0,1000);

	gr_0 -> GetXaxis() -> SetLimits(0,1000);
	gr_1 -> GetXaxis() -> SetLimits(0,1000);

	gr_0 -> GetHistogram() -> SetMaximum(500);
	gr_0 -> GetHistogram() -> SetMinimum(0);
	gr_1 -> GetHistogram() -> SetMaximum(500);
	gr_1 -> GetHistogram() -> SetMinimum(0);

	

	gr_0 -> Draw("AP z");
	gr_1 -> Draw("P SAME");

	TF1 * fun_adc0 = new TF1("fun_adc0",myFun, MINS1,MAXS1,3);
	fun_adc0 -> SetLineColor(kRed);
	TF1 * fun_adc1 = new TF1("fun_adc1",myFun, MINS2,MAXS2,3);
	fun_adc1 -> SetLineColor(kBlue);

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

	c_5 -> Modified();
	c_5 -> Update();
	TPaveStats *st0 = (TPaveStats*) gr_0->FindObject("stats");

	if (st0 == NULL) {
		cout << "st0 is nullptr!!!!" << endl;
	} else {
		st0 -> SetX1NDC(0.3); //new x start position
		st0 -> SetX2NDC(0.4); //new x end position
		c_5 -> Modified();
		c_5 -> Update();
	}


	c_5 -> Modified();
	c_5 -> Update();

	//correzione amplitude walk
	TCanvas* c_corretto = new TCanvas("c_corretto","c_corretto",0,0,700,500);
	TGraphErrors * gr_0v2 = new TGraphErrors;
	TGraphErrors * gr_1v2 = new TGraphErrors;

	gr_0v2 -> GetXaxis() -> SetTitle("canali ADC");
	gr_0v2 -> GetYaxis() -> SetTitle("canali TDC");
	gr_0v2 -> SetMarkerStyle(20);
	gr_0v2 -> SetMarkerSize(0.5);
	gr_0v2 -> SetMarkerColor(kRed);

	gr_1v2 -> GetXaxis() -> SetTitle("canali ADC");
	gr_1v2 -> GetYaxis() -> SetTitle("canali TDC");
	gr_1v2 -> SetMarkerStyle(20);
	gr_1v2 -> SetMarkerSize(0.5);
	gr_1v2 -> SetMarkerColor(kBlue);


	double tau0 = fun_adc0 -> GetParameter(0);
	double vth0 = fun_adc0 -> GetParameter(1);
	double off0 = fun_adc0 -> GetParameter(2);

	double tau1 = fun_adc1 -> GetParameter(0);
	double vth1 = fun_adc1 -> GetParameter(1);
	double off1 = fun_adc1 -> GetParameter(2);



	for (int i = 0; i < NBIN; i++){
		double x1 = th_1 -> GetXaxis () -> GetBinCenter(i);
		double x2 = th_2 -> GetXaxis () -> GetBinCenter(i);

		if (x1 > MINS1 && x1 < MAXS1){
			double min = (x1 - delta1)/2;
			double max = (x1 + delta1)/2;
			double mean = 0;
			double std = 0;
			double count = 0;
			for(int i = 0; i < ampiezzas1.size(); i++){
				double x = ampiezzas1[i];
				if (x > min && x < max){
					mean = mean + tempi[i];
					count++;
				}
			}
			mean = mean / count;
			for(int i = 0; i < ampiezzas1.size(); i++){
				double x = ampiezzas1[i];
				if (x > min && x < max){
					if (count > 1) std = std + (x - mean)*(x - mean)/(count - 1);
					else std = 0;
				}
			}
			std = sqrt(std) / sqrt(count);
			double y = mean;
			int n = gr_0v2 -> GetN();
			y = y - tau0 * log(x1/(x1-vth0)) - off0;
			gr_0v2 -> SetPoint(n, x1, y);
			gr_0v2 -> SetPointError(n, 0, std);

		}

		if (x2 > MINS2 && x2 < MAXS2){
			double min = (x2 - delta2)/2;
			double max = (x2 + delta2)/2;
			double mean = 0;
			double std = 0;
			double count = 0;
			for(int i = 0; i < ampiezzas2.size(); i++){
				double x = ampiezzas2[i];
				if (x > min && x < max){
					mean = mean + tempi[i];
					count++;
				}
			}
			mean = mean / count;
			for(int i = 0; i < ampiezzas2.size(); i++){
				double x = ampiezzas2[i];
				if (x > min && x < max){
					if (count > 1) std = std + (x - mean)*(x - mean)/(count - 1);
					else std = 0;
				}
			}
			std = sqrt(std) / sqrt(count);
			double y = mean;
			y = y - tau1 * log(x2/(x2-vth1)) - off1;
			int n = gr_1v2 -> GetN();
			gr_1v2 -> SetPoint(n, x2, y);
			gr_1v2 -> SetPointError(n, 0, std);
			
		}
	}

	c_corretto -> cd();

	gr_0v2 -> GetXaxis() -> SetRangeUser(0,1000);
	gr_1v2 -> GetXaxis() -> SetRangeUser(0,1000);

	gr_0v2 -> GetXaxis() -> SetLimits(0,1000);
	gr_1v2 -> GetXaxis() -> SetLimits(0,1000);

	gr_0v2 -> GetHistogram() -> SetMaximum(500);
	gr_0v2 -> GetHistogram() -> SetMinimum(0);
	gr_1v2 -> GetHistogram() -> SetMaximum(500);
	gr_1v2 -> GetHistogram() -> SetMinimum(0);

	

	gr_0v2 -> Draw("AP");
	gr_1v2 -> Draw("P SAME");


	myApp.Run();
	return 0;
}
