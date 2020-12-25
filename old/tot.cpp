//c++ -o tot tot.cpp `root-config --cflags --glibs`
// ./tot /mnt/c/Users/Rodolfo/Desktop/calibrazione/3/ 

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
#include "TFitResult.h"
#include "TMatrixD.h"

using namespace std;

#define DEBUG false

#define AUTOLIMIT true
#define AUTOBIN true

#define ADC0_FILE "datiADC0.txt"
#define ADC1_FILE "datiADC1.txt"
#define TDC_FILE "datiTDC.txt"
#define SCA_FILE "datiscaler.txt"
#define OUT_FILE "output.txt"

#define MINS1 10
#define MAXS1 60
#define MINS2 10
#define MAXS2 300

#define CHAN_ADC0 1
#define CHAN_ADC1 1
#define CHAN_TDC  1

#define CHAN_SING0  1
#define CHAN_SING1  2
#define CHAN_DOPP   5

#define NBIN 100
#define BINTEMPI 100

#define DISTANZA 0.86
#define E_DIST 0.02

#define CONVERSIONETDC 0.1197 + 2.116
#define RITARDO 40.6

bool leggi_dati(vector<double> & ampiezzas1, int channel, char* myFile){
	double scal1, scal2, scal3, scal4, scal5, scal6, scal7, scal8;
	ifstream Infile;
	Infile.open(myFile, fstream::in);
	if (Infile.good () == false){
		cout << "Errore nell'apertura dei file " << myFile << endl;
		return false;
	}
	cout << "Leggo file di dati " << myFile << endl;
	while(true){
		//Infile >> scal1 >> scal2 >> scal3 >> scal4 >> scal5 >> scal6 >> scal7 >> scal8;
		Infile >> scal1;
		switch(channel){
			case 1: 
				ampiezzas1.push_back(scal1);
				break;
			case 2: 
				ampiezzas1.push_back(scal2);
				break;
			case 3: 
				ampiezzas1.push_back(scal3);
				break;
			case 4: 
				ampiezzas1.push_back(scal4);
				break;
			case 5: 
				ampiezzas1.push_back(scal5);
				break;
			case 6: 
				ampiezzas1.push_back(scal6);
				break;
			case 7: 
				ampiezzas1.push_back(scal7);
				break;
			case 8: 
				ampiezzas1.push_back(scal8);
				break;	
			default:
				cout << "\nLa colonna nel define e' errata" << endl;
				return 1;
				break;
		}
		if (Infile.eof() == true) break;
	}
	Infile.close();
	cout << "Numero dati: " << ampiezzas1.size() << endl;
	return true;
}

bool leggi_datiSCA(vector<int> & singola0, vector<int> & singola1, vector<int> & doppie, int channel_sing0, int channel_sing1, int channel_doppie, char* myFile){
	int scal1, scal2, scal3, scal4, scal5, scal6, scal7, scal8, scal9, scal10, scal11, scal12;
	ifstream Infile;
	Infile.open(myFile, fstream::in);
	if (Infile.good () == false){
		cout << "Errore nell'apertura dei file " << myFile << endl;
		return false;
	}
	cout << "Leggo file di dati " << myFile << endl;
	int count0 = 0;
	bool counted0 = false;
	int count1 = 0;
	bool counted1 = false;
	int count2 = 0;
	bool counted2 = false;
	while(true){
		Infile >> scal1 >> scal2 >> scal3 >> scal4 >> scal5 >> scal6 >> scal7 >> scal8 >> scal9 >> scal10 >> scal11 >> scal12;
		switch(channel_sing0){
			case 1: 
				if(scal1 > 10000000 && counted0 == false){
					count0++;
					counted0 = true;
				}
				if(scal1 < 10000000 && counted0 == true) counted0 = false;
				scal1 = scal1 + count0 * 16777216;
				singola0.push_back(scal1);
				break;
			case 2: 
				if(scal2 > 10000000 && counted0 == false){
					count0++;
					counted0 = true;
				}
				if(scal2 < 10000000 && counted0 == true) counted0 = false;
				scal2 = scal2 + count0 * 16777216;
				singola0.push_back(scal2);
				break;
			case 3: 
				if(scal3 > 10000000 && counted0 == false){
					count0++;
					counted0 = true;
				}
				if(scal3 < 10000000 && counted0 == true) counted0 = false;
				scal3 = scal3 + count0 * 16777216;
				singola0.push_back(scal3);
				break;
			case 4: 
				if(scal4 > 10000000 && counted0 == false){
					count0++;
					counted0 = true;
				}
				if(scal4 < 10000000 && counted0 == true) counted0 = false;
				scal4 = scal4 + count0 * 16777216;
				singola0.push_back(scal4);
				break;
			case 5: 
				if(scal5 > 10000000 && counted0 == false){
					count0++;
					counted0 = true;
				}
				if(scal5 < 10000000 && counted0 == true) counted0 = false;
				scal5 = scal5 + count0 * 16777216;
				singola0.push_back(scal5);
				break;
			case 6: 
				if(scal6 > 10000000 && counted0 == false){
					count0++;
					counted0 = true;
				}
				if(scal6 < 10000000 && counted0 == true) counted0 = false;
				scal6 = scal6 + count0 * 16777216;
				singola0.push_back(scal6);
				break;
			case 7: 
				if(scal7 > 10000000 && counted0 == false){
					count0++;
					counted0 = true;
				}
				if(scal7 < 10000000 && counted0 == true) counted0 = false;
				scal7 = scal7 + count0 * 16777216;
				singola0.push_back(scal7);
				break;
			case 8: 
				if(scal8 > 10000000 && counted0 == false){
					count0++;
					counted0 = true;
				}
				if(scal8 < 10000000 && counted0 == true) counted0 = false;
				scal8 = scal8 + count0 * 16777216;
				singola0.push_back(scal8);
				break;	
			case 9: 
				if(scal9 > 10000000 && counted0 == false){
					count0++;
					counted0 = true;
				}
				if(scal9 < 10000000 && counted0 == true) counted0 = false;
				scal9 = scal9 + count0 * 16777216;
				singola0.push_back(scal9);
				break;
			case 10: 
				if(scal10 > 10000000 && counted0 == false){
					count0++;
					counted0 = true;
				}
				if(scal10 < 10000000 && counted0 == true) counted0 = false;
				scal10 = scal10 + count0 * 16777216;
				singola0.push_back(scal10);
				break;
			case 11: 
				if(scal11 > 10000000 && counted0 == false){
					count0++;
					counted0 = true;
				}
				if(scal11 < 10000000 && counted0 == true) counted0 = false;
				scal11 = scal11 + count0 * 16777216;
				singola0.push_back(scal11);
				break;
			case 12: 
				if(scal12 > 10000000 && counted0 == false){
					count0++;
					counted0 = true;
				}
				if(scal12 < 10000000 && counted0 == true) counted0 = false;
				scal12 = scal12 + count0 * 16777216;
				singola0.push_back(scal12);
				break;	
			default:
				cout << "\nLa colonna nel define e' errata" << endl;
				return 1;
				break;
		}
		switch(channel_sing1){
			case 1: 
				if(scal1 > 10000000 && counted1 == false){
					count1++;
					counted1 = true;
				}
				if(scal1 < 10000000 && counted1 == true) counted1 = false;
				scal1 = scal1 + count1 * 16777216;
				singola1.push_back(scal1);
				break;
			case 2: 
				if(scal2 > 10000000 && counted1 == false){
					count1++;
					counted1 = true;
				}
				if(scal2 < 10000000 && counted1 == true) counted1 = false;
				scal2 = scal2 + count1 * 16777216;
				singola1.push_back(scal2);
				break;
			case 3: 
				if(scal3 > 10000000 && counted1 == false){
					count1++;
					counted1 = true;
				}
				if(scal3 < 10000000 && counted1 == true) counted1 = false;
				scal3 = scal3 + count1 * 16777216;
				singola1.push_back(scal3);
				break;
			case 4: 
				if(scal4 > 10000000 && counted1 == false){
					count1++;
					counted1 = true;
				}
				if(scal4 < 10000000 && counted1 == true) counted1 = false;
				scal4 = scal4 + count1 * 16777216;
				singola1.push_back(scal4);
				break;
			case 5: 
				if(scal5 > 10000000 && counted1 == false){
					count1++;
					counted1 = true;
				}
				if(scal5 < 10000000 && counted1 == true) counted1 = false;
				scal5 = scal5 + count1 * 16777216;
				singola1.push_back(scal5);
				break;
			case 6: 
				if(scal6 > 10000000 && counted1 == false){
					count1++;
					counted1 = true;
				}
				if(scal6 < 10000000 && counted1 == true) counted1 = false;
				scal6 = scal6 + count1 * 16777216;
				singola1.push_back(scal6);
				break;
			case 7: 
				if(scal7 > 10000000 && counted1 == false){
					count1++;
					counted1 = true;
				}
				if(scal7 < 10000000 && counted1 == true) counted1 = false;
				scal7 = scal7 + count1 * 16777216;
				singola1.push_back(scal7);
				break;
			case 8: 
				if(scal8 > 10000000 && counted1 == false){
					count1++;
					counted1 = true;
				}
				if(scal8 < 10000000 && counted1 == true) counted1 = false;
				scal8 = scal8 + count1 * 16777216;
				singola1.push_back(scal8);
				break;	
			case 9: 
				if(scal9 > 10000000 && counted1 == false){
					count1++;
					counted1 = true;
				}
				if(scal9 < 10000000 && counted1 == true) counted1 = false;
				scal9 = scal9 + count1 * 16777216;
				singola1.push_back(scal9);
				break;
			case 10: 
				if(scal10 > 10000000 && counted1 == false){
					count1++;
					counted1 = true;
				}
				if(scal10 < 10000000 && counted1 == true) counted1 = false;
				scal10 = scal10 + count1 * 16777216;
				singola1.push_back(scal10);
				break;
			case 11: 
				if(scal11 > 10000000 && counted1 == false){
					count1++;
					counted1 = true;
				}
				if(scal11 < 10000000 && counted1 == true) counted1 = false;
				scal11 = scal11 + count1 * 16777216;
				singola1.push_back(scal11);
				break;
			case 12: 
				if(scal12 > 10000000 && counted1 == false){
					count1++;
					counted1 = true;
				}
				if(scal12 < 10000000 && counted1 == true) counted1 = false;
				scal12 = scal12 + count1 * 16777216;
				singola1.push_back(scal12);
				break;	
			default:
				cout << "\nLa colonna nel define e' errata" << endl;
				return 1;
				break;
		}
		switch(channel_doppie){
			case 1: 
				if(scal1 > 10000000 && counted2 == false){
					count2++;
					counted2 = true;
				}
				if(scal1 < 10000000 && counted2 == true) counted2 = false;
				scal1 = scal1 + count2 * 16777216;
				doppie.push_back(scal1);
				break;
			case 2: 
				if(scal2 > 10000000 && counted2 == false){
					count2++;
					counted2 = true;
				}
				if(scal2 < 10000000 && counted2 == true) counted2 = false;
				scal2 = scal2 + count2 * 16777216;
				doppie.push_back(scal2);
				break;
			case 3: 
				if(scal3 > 10000000 && counted2 == false){
					count2++;
					counted2 = true;
				}
				if(scal3 < 10000000 && counted2 == true) counted2 = false;
				scal3 = scal3 + count2 * 16777216;
				doppie.push_back(scal3);
				break;
			case 4: 
				if(scal4 > 10000000 && counted2 == false){
					count2++;
					counted2 = true;
				}
				if(scal4 < 10000000 && counted2 == true) counted2 = false;
				scal4 = scal4 + count2 * 16777216;
				doppie.push_back(scal4);
				break;
			case 5: 
				if(scal5 > 10000000 && counted2 == false){
					count2++;
					counted2 = true;
				}
				if(scal5 < 10000000 && counted2 == true) counted2 = false;
				scal5 = scal5 + count2 * 16777216;
				doppie.push_back(scal5);
				break;
			case 6: 
				if(scal6 > 10000000 && counted2 == false){
					count2++;
					counted2 = true;
				}
				if(scal6 < 10000000 && counted2 == true) counted2 = false;
				scal6 = scal6 + count2 * 16777216;
				doppie.push_back(scal6);
				break;
			case 7: 
				if(scal7 > 10000000 && counted2 == false){
					count2++;
					counted2 = true;
				}
				if(scal7 < 10000000 && counted2 == true) counted2 = false;
				scal7 = scal7 + count2 * 16777216;
				doppie.push_back(scal7);
				break;
			case 8: 
				if(scal8 > 10000000 && counted2 == false){
					count2++;
					counted2 = true;
				}
				if(scal8 < 10000000 && counted2 == true) counted2 = false;
				scal8 = scal8 + count2 * 16777216;
				doppie.push_back(scal8);
				break;	
			case 9: 
				if(scal9 > 10000000 && counted2 == false){
					count2++;
					counted2 = true;
				}
				if(scal9 < 10000000 && counted2 == true) counted2 = false;
				scal9 = scal9 + count2 * 16777216;
				doppie.push_back(scal9);
				break;
			case 10: 
				if(scal10 > 10000000 && counted2 == false){
					count2++;
					counted2 = true;
				}
				if(scal10 < 10000000 && counted2 == true) counted2 = false;
				scal10 = scal10 + count2 * 16777216;
				doppie.push_back(scal10);
				break;
			case 11: 
				if(scal11 > 10000000 && counted2 == false){
					count2++;
					counted2 = true;
				}
				if(scal11 < 10000000 && counted2 == true) counted2 = false;
				scal11 = scal11 + count2 * 16777216;
				doppie.push_back(scal11);
				break;
			case 12: 
				if(scal12 > 10000000 && counted2 == false){
					count2++;
					counted2 = true;
				}
				if(scal12 < 10000000 && counted2 == true) counted2 = false;
				scal12 = scal12 + count2 * 16777216;
				doppie.push_back(scal12);
				break;	
			default:
				cout << "\nLa colonna nel define e' errata" << endl;
				return 1;
				break;
		}
		if (Infile.eof() == true) break;
	}
	Infile.close();
	cout << "Numero dati: " << singola0.size() << endl;
	cout << "Numero dati: " << singola1.size() << endl;
	cout << "Numero dati: " << doppie.size() << endl;
	return true;
}

bool scrivi_dati(vector<double> & adc0, vector<double> & adc1, vector<double> & tdc, char * file){
	cout << "Apro il file: " << file << endl;
	ofstream myfile;
  	myfile.open (file);
  	for(int i = 0; i< adc0.size(); i++){
  		myfile << tdc[i] << " " << adc0[i] << " " << adc1[i] << endl;
  	}
  	myfile.close();
	return true;
}

double myFun(double *x, double *par){		
	double A = par[0];		//tau
	double B = par[1];		//Vth
	double C = par[2];		//offset
	double D = x[0];
	return A*log(D/(D-B))+C;
}

void setlimits(TH1D * myhisto, double * min, double * max){	
	TF1 * g = new TF1("g","landau",85,95);	
	myhisto -> Fit(g, "N Q");
	double mean = g -> GetParameter (1);
	double sigma = g -> GetParameter (2);
	*min = mean - 1.5 * sigma;
	*max = mean + 1.5 * sigma;
	cout << "media: " << mean << "  sigma: " << sigma << endl;
	return;
}

double propagazione(double x, double errx, double erry, double vth, double tau, double errvth, double errtau, double cov_tau_vth){
	double v_y = pow(erry,2);
	double v_x = pow(errx,2);
	double v_tau = pow(errtau,2);
	double v_vth = pow(errvth,2);

	double v_diff = v_x + v_vth;
	double v_frac = v_x / pow(x - vth,2) + pow(x,2)*v_diff/pow(x-vth,4)-2*x/pow(x-vth,3);

	double A = v_y;
	double B = pow(log(x/(x-vth)),2)*v_tau;
	double C = pow(tau/(x/(x-vth)),2)*v_frac;
	double D = -2 / (x - vth) * cov_tau_vth;

	double fin = A + B + C + D;

	return  sqrt(fin);
}

int main(int argc, char *argv[]){

	if (DEBUG == true)	cout << "\nLETTURA DEI FILE" << endl;
	//----------------------------------------------------------------------
	// 						    lettura dei file
	//----------------------------------------------------------------------

	if(argc < 2){
		cout << "Devi inserire la directory dei dati" << endl;
		return 1;
	}
	double dist;
	if(argc < 3) dist = DISTANZA;
	else dist = stod(argv[2]);

	double maxs1 = 0;
	double maxs2 = 0;
	vector<double> ampiezzas1;
	vector<double> ampiezzas2;
	vector<double> tempi;

	vector<int> singola0;
	vector<int> singola1;
	vector<int> doppie;

	string path = argv[1];
	string path_adc0 = path + ADC0_FILE;
	string path_adc1 = path + ADC1_FILE;
	string path_tdc = path + TDC_FILE;
	string path_sca = path + SCA_FILE;
	string path_out = path + OUT_FILE;

	if(!(leggi_dati(ampiezzas1, CHAN_ADC0, &path_adc0[0]))) return 1;
	if(!(leggi_dati(ampiezzas2, CHAN_ADC1, &path_adc1[0]))) return 1;
	if(!(leggi_dati(tempi, CHAN_TDC, &path_tdc[0]))) return 1;
	if(!(leggi_datiSCA(singola0, singola1, doppie, CHAN_SING0, CHAN_SING1, CHAN_DOPP, &path_sca[0]))) return 1;

	if(!(scrivi_dati(ampiezzas1, ampiezzas2, tempi, &path_out[0]))) return 1;
	cout << "Done" << endl;

	
	if (DEBUG == true)	cout << "\nVISUALIZATION" << endl;
	//----------------------------------------------------------------------
	// 						      visualization
	//----------------------------------------------------------------------

	TApplication myApp("myApp",NULL,NULL);
	gStyle -> SetOptFit(1111);
	gStyle -> SetOptStat(1111);
	if (DEBUG == true)	cout << "\nISTOGRAMMI ADC" << endl;
	//----------------------------------------------------------------------
	// 						      istogrammi adc
	//----------------------------------------------------------------------

	TCanvas* c_1 = new TCanvas("c_1","c_1",0,0,700,500);
	TCanvas* c_2 = new TCanvas("c_2","c_2",0,0,700,500);

	for(int i = 0; i < ampiezzas1.size(); i++){
		if (ampiezzas1[i] > maxs1) maxs1 = ampiezzas1[i];
	}
	cout << "\nAUTOBIN: " << AUTOBIN << endl;
	int nbin1;
	if(AUTOBIN == true) nbin1 = maxs1;
	else nbin1 = NBIN;
	TH1D* th_1 = new TH1D("ADC S1", "ADC S1", nbin1, 0, maxs1);	
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
	int nbin2;
	if(AUTOBIN == true) nbin2 = maxs2;
	else nbin2 = NBIN;
	TH1D* th_2 = new TH1D("ADC S2", "ADC S2", nbin2, 0, maxs2);	
	for(int i = 0; i < ampiezzas1.size(); i++){
		if(ampiezzas2[i] != 0 && ampiezzas2[i] < 1000) th_2 -> Fill(ampiezzas2[i]);
	}
	c_2->cd();
	th_2 -> SetFillColor(kCyan);
	th_2 -> Draw();
	cout << "Il massimo di S1 si trova a x = " << th_2 -> GetXaxis() -> GetBinCenter(th_2 -> GetMaximumBin()) << endl;

	double delta1 = (th_1 -> GetXaxis () -> GetBinCenter(20) - th_1 -> GetXaxis () -> GetBinCenter(19));
	double delta2 = (th_2 -> GetXaxis () -> GetBinCenter(20) - th_2 -> GetXaxis () -> GetBinCenter(19));

	double mins1_lan;
	double maxs1_lan;
	double mins2_lan;
	double maxs2_lan;

	cout << "\n\nAUTOLIMIT: " << AUTOLIMIT << endl;
	if(AUTOLIMIT == true){
		setlimits(th_1, & mins1_lan, & maxs1_lan);
		setlimits(th_2, & mins2_lan, & maxs2_lan);
	}
	else{
		mins1_lan = MINS1;
		maxs2_lan = MAXS2;
		mins2_lan = MINS2;
		maxs1_lan = MAXS1;
	}
	cout << "S1: " << mins1_lan << " < x < " << maxs1_lan << endl;
	cout << "S2: " << mins2_lan << " < x < " << maxs2_lan << endl;
	cout << endl;
	
	if (DEBUG == true)	cout << "\nGRAFICO INIZIALE" << endl;
	//----------------------------------------------------------------------
	// 						   grafico iniziale
	//----------------------------------------------------------------------

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
																							if (DEBUG == true)	cout << __LINE__ << endl;
	vector<double> meanx;
	vector<double> stdx;
	for (int i = 0; i < nbin1; i++){
		double x1 = th_1 -> GetXaxis () -> GetBinCenter(i);
		double min = x1 - delta1/2;
		double max = x1 + delta1/2;
		double media = 0;
		double std = 0;
		int count = 0;
		for(int j = 0; j < ampiezzas1.size(); j++){
			double x = ampiezzas1[j];
			if (x >= min && x < max){
					media = media + x;
					count++;
			}
		}
		media = media / count;
		for(int j = 0; j < ampiezzas1.size(); j++){
			double x = ampiezzas1[j];
			if (x > min && x < max){
				if (count > 1) std = std + (x - media)*(x - media)/(count - 1);
				else std = 0;
			}
		}
		if(count > 0){
			std = std / sqrt(count);
			std = 0.001 * media;
			meanx.push_back(media);
			stdx.push_back(std);
		}
		else{
			meanx.push_back(-10);
			stdx.push_back(-10);			
		}

	}
																							if (DEBUG == true)	cout << __LINE__ << endl;

	vector<double> meanx2;
	vector<double> stdx2;
	for (int i = 0; i < nbin2; i++){
		double x1 = th_2 -> GetXaxis () -> GetBinCenter(i);
		double min = x1 - delta2/2;
		double max = x1 + delta2/2;
		double media = 0;
		double std = 0;
		int count = 0;
		for(int j = 0; j < ampiezzas2.size(); j++){
			double x = ampiezzas2[j];
			if (x >= min && x < max){
					media = media + x;
					count++;
			}
		}
		media = media / count;
		for(int j = 0; j < ampiezzas2.size(); j++){
			double x = ampiezzas2[j];
			if (x > min && x < max){
				if (count > 1) std = std + (x - media)*(x - media)/(count - 1);
				else std = 0;
			}
		}
		if(count > 0){
			std = std / sqrt(count);
			std = 0.001 * media;
			meanx2.push_back(media);
			stdx2.push_back(std);
		}
		else{
			meanx2.push_back(-10);
			stdx2.push_back(-10);
		}

	}
	vector<double> meany;
	vector<double> stdy;
																							if (DEBUG == true)	cout << __LINE__ << endl;
	for (int i = 0; i < nbin1; i++){
		double x1 = th_1 -> GetXaxis () -> GetBinCenter(i);
		if (x1 > mins1_lan && x1 < maxs1_lan){
			double min = x1 - delta1/2;
			double max = x1 + delta1/2;
			double mean = 0;
			double std = 0;
			double count = 0;
																							if (DEBUG == true)	cout << __LINE__ << endl;
			for(int j = 0; j < ampiezzas1.size(); j++){
				double x = ampiezzas1[j];
				if (x >= min && x < max){
					mean = mean + tempi[j];
					count++;
				}
			}
			mean = mean / count;
																							if (DEBUG == true)	cout << __LINE__ << endl;
			for(int j = 0; j < ampiezzas1.size(); j++){
				double x = ampiezzas1[j];
				if (x >= min && x < max){
					if (count > 1){
						x = tempi[j];
						std = std + (x - mean)*(x - mean)/(count - 1);
					}
					else std = 1.72;
				}
			}
																							if (DEBUG == true)	cout << __LINE__ << endl;
			std = sqrt(std) / sqrt(count);
																							if (DEBUG == true)	cout << __LINE__ << endl;
			double y = mean;
																							if (DEBUG == true)	cout << __LINE__ << endl;
			x1 = meanx[i];
																							if (DEBUG == true)	cout << __LINE__ << endl;
			int n = gr_0 -> GetN();
																							if (DEBUG == true)	cout << __LINE__ << endl;
			if (count > 0 && x1 > 0) gr_0 -> SetPoint(n, x1, y);
			meany.push_back(y);
			stdy.push_back(std);
																							if (DEBUG == true)	cout << __LINE__ << endl;
			double xerr = stdx[i];																							
																							if (DEBUG == true)	cout << __LINE__ << endl;
			if(xerr == 0) xerr = 1;
																							if (DEBUG == true)	cout << __LINE__ << endl;
			if(std == 0) std = 1;
																							if (DEBUG == true)	cout << __LINE__ << endl;
			if (count > 0 && x1 > 0) gr_0 -> SetPointError(n, xerr, std);
																							if (DEBUG == true)	cout << __LINE__ << endl;

		}
	}
	vector<double> meany2;
	vector<double> stdy2;
																							if (DEBUG == true)	cout << __LINE__ << endl;
	for (int i = 0; i < nbin2; i++){
		double x2 = th_2 -> GetXaxis () -> GetBinCenter(i);
		if (x2 > mins2_lan && x2 < maxs2_lan){
			double min = x2 - delta2/2;
			double max = x2 + delta2/2;
			double mean = 0;
			double std = 0;
			double count = 0;
			for(int j = 0; j < ampiezzas2.size(); j++){
				double x = ampiezzas2[j];
				if (x >= min && x < max){
					mean = mean + tempi[j];
					count++;
				}
			}
			mean = mean / count;
			for(int j = 0; j < ampiezzas2.size(); j++){
				double x = ampiezzas2[j];
				if (x >= min && x < max){
					if (count > 1){
						x = tempi[j]; 
						std = std + (x - mean)*(x - mean)/(count - 1);
					}
					else std = 1.72;
				}
			}
			std = sqrt(std) / sqrt(count);
			double y = mean;
			x2 = meanx2[i];
			int n = gr_1 -> GetN();
			if (count > 0 && x2 > 0) gr_1 -> SetPoint(n, x2, y);
			meany2.push_back(y);
			stdy2.push_back(std);
			double xerr = stdx2[i];
			if(xerr == 0) xerr = 1;
			if(std == 0) std = 1;
			if (count > 0 && x2 > 0) gr_1 -> SetPointError(n, xerr, std);
			
		}
	}
																							if (DEBUG == true)	cout << __LINE__ << endl;

	c_5 -> cd();

	gr_0 -> GetXaxis() -> SetRangeUser(0,300);
	gr_1 -> GetXaxis() -> SetRangeUser(0,300);

	gr_0 -> GetXaxis() -> SetLimits(0,300);
	gr_1 -> GetXaxis() -> SetLimits(0,300);

	gr_0 -> GetHistogram() -> SetMaximum(340);
	gr_0 -> GetHistogram() -> SetMinimum(240);
	gr_1 -> GetHistogram() -> SetMaximum(340);
	gr_1 -> GetHistogram() -> SetMinimum(240);

	gr_0 -> Draw("AP z");
	gr_1 -> Draw("P z SAME");

	if (DEBUG == true)	cout << "\nFIT" << endl;
	//----------------------------------------------------------------------
	// 						         fit
	//----------------------------------------------------------------------

	TF1 * fun_adc0 = new TF1("fun_adc0",myFun, mins1_lan,maxs1_lan,3);
	fun_adc0 -> SetLineColor(kRed);
	TF1 * fun_adc1 = new TF1("fun_adc1",myFun, mins2_lan,maxs2_lan,3);
	fun_adc1 -> SetLineColor(kBlue);

	fun_adc0 -> SetParName(0, "Tau0");
	fun_adc0 -> SetParName(1, "Vth0");
	fun_adc0 -> SetParName(2, "Off0");

	fun_adc1 -> SetParName(0, "Tau1");
	fun_adc1 -> SetParName(1, "Vth1");
	fun_adc1 -> SetParName(2, "Off1");

	fun_adc0 -> SetParameter(0, 13);
	fun_adc0 -> SetParameter(1, -50);
	fun_adc0 -> SetParameter(2, 300);

	fun_adc1 -> SetParameter(0, 34);
	fun_adc1 -> SetParameter(1, -50);
	fun_adc1 -> SetParameter(2, 300);

	TFitResultPtr r_0 = gr_0 -> Fit("fun_adc0", "R S");
	TFitResultPtr r_1 = gr_1 -> Fit("fun_adc1", "R S");

	c_5 -> Modified();
	c_5 -> Update();
	TPaveStats *st0 = (TPaveStats*) gr_0->FindObject("stats");

	if (st0 == NULL) {
		cout << "st0 is nullptr!!!!" << endl;
	} else {
		st0 -> SetX1NDC(0.3); //new x start position
		st0 -> SetX2NDC(0.6); //new x end position
		c_5 -> Modified();
		c_5 -> Update();
	}


	c_5 -> Modified();
	c_5 -> Update();

	if (DEBUG == true)	cout << "\nGRAFICO CORRETTO" << endl;
	//----------------------------------------------------------------------
	// 						   grafico corretto
	//----------------------------------------------------------------------

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
	double e_tau0 = fun_adc0 -> GetParError(0);
	double vth0 = fun_adc0 -> GetParameter(1);
	double e_vth0 = fun_adc0 -> GetParError(1);
	double off0 = fun_adc0 -> GetParameter(2);

	double tau1 = fun_adc1 -> GetParameter(0);
	double e_tau1 = fun_adc1 -> GetParError(0);
	double vth1 = fun_adc1 -> GetParameter(1);
	double e_vth1 = fun_adc1 -> GetParError(1);
	double off1 = fun_adc1 -> GetParameter(2);


	TMatrixD cor_0 = r_0 -> GetCorrelationMatrix();
	TMatrixD cov_0 = r_0 -> GetCovarianceMatrix();

	TMatrixD cor_1 = r_1 -> GetCorrelationMatrix();
	TMatrixD cov_1 = r_1 -> GetCovarianceMatrix();

	double cov_tv_0 = cov_0(0,1);
	double cov_tv_1 = cov_1(0,1);

	cov_1.Print();
	cov_0.Print();

																							if (DEBUG == true)	cout << __LINE__ << endl;

	for(int i = mins1_lan; i <= maxs1_lan; i++){
		int count = 0;
		double x1 = 0;
		double x2 = 0;
		double tempo = 0;
		for(int j = 0; j < ampiezzas1.size(); j++){
			double xtemp = ampiezzas1[j];
			if ((int)xtemp == i){
				count++;
				x1 = x1 + ampiezzas1[j];
				x2 = x2 + ampiezzas2[j];
				tempo = tempo + tempi[i];
			}
		}
		x1 = x1 / count;
		x2 = x2 / count;
		tempo = tempo / count;
		double adc0 = tau0 * log (x1 / (x1 - vth0));
		double adc1 = tau1 * log (x2 / (x2 - vth1));
		double tdc = tempo - adc0 + adc1;
		int N = gr_0v2 -> GetN();
		gr_0v2 -> SetPoint(N, x1, tdc);
		cout << "punto " << N << " " << x1 << " " << tdc << "\t" << adc0 << " " << adc1 << endl;
	}			

	for(int i = mins2_lan; i <= maxs2_lan; i++){
		int count = 0;
		double x1 = 0;
		double x2 = 0;
		double tempo = 0;
		for(int j = 0; j < ampiezzas2.size(); j++){
			double xtemp = ampiezzas2[j];
			if ((int)xtemp == i){
				count++;
				x1 = x1 + ampiezzas1[j];
				x2 = x2 + ampiezzas2[j];
				tempo = tempo + tempi[i];
			}
		}
		x1 = x1 / count;
		x2 = x2 / count;
		tempo = tempo / count;
		double adc0 = tau0 * log (x1 / (x1 - vth0));
		double adc1 = tau1 * log (x2 / (x2 - vth1));
		double tdc = tempo - adc0 + adc1;
		int N = gr_1v2 -> GetN();
		gr_1v2 -> SetPoint(N, x2, tdc);
	}	







/*
	for(int i = 0; i < ampiezzas1.size(); i++){
		double x1 = ampiezzas1[i];
		double x2 = ampiezzas2[i];
		double adc0 = tau0 * log (x1 / (x1 - vth0));
		double adc1 = tau1 * log (x2 / (x2 - vth1));
		double tdc = tempi[i] - adc0 + adc1;
		if (x1 >= mins1_lan && x1 < maxs1_lan){
			int N = gr_0v2 -> GetN();
			gr_0v2 -> SetPoint(N, x1, tdc);
		}
		if (x2 >= mins2_lan && x2 < maxs2_lan){
			int N = gr_1v2 -> GetN();
			gr_1v2 -> SetPoint(N, x2, tdc);
		}
	}																				



	for(int i = 0; i < gr_0 -> GetN(); i++){
		double x;
		double y;
		double xerr;
		double yerr;
		double x2;
		gr_1 -> GetPoint(i,x2,y);
		gr_0 -> GetPoint(i, x, y);
		xerr = gr_0 -> GetErrorX(i);
		yerr = gr_0 -> GetErrorY(i);
		y = y - tau0 * log(x/(x-vth0)) + tau1 * log(x2/(x2-vth1)) ;
		yerr = propagazione(x,xerr,yerr,vth0, tau0, e_vth0, e_tau0, cov_tv_0);				
		gr_0v2 -> SetPoint(i, x, y);
		gr_0v2 -> SetPointError(i, xerr, yerr);
	}
																							if (DEBUG == true)	cout << __LINE__ << endl;
	for (int i = 0; i < gr_1 -> GetN(); i++){
		double x;
		double y;
		double xerr;
		double yerr;
		double x1;
		gr_0 -> GetPoint(i,x1,y);
		gr_1 -> GetPoint(i, x, y);
		xerr = gr_1 -> GetErrorX(i);
		yerr = gr_1 -> GetErrorY(i);
		y = y - tau0 * log(x1/(x1-vth0)) + tau1 * log(x/(x-vth1)) ;
		yerr = propagazione(x,xerr,yerr,vth1, tau1, e_vth1, e_tau1, cov_tv_1);	
		gr_1v2 -> SetPoint(i, x, y);
		gr_1v2 -> SetPointError(i, xerr, yerr);
	}*/
																							if (DEBUG == true)	cout << __LINE__ << endl;

	c_corretto -> cd();

	gr_0v2 -> GetXaxis() -> SetRangeUser(0,300);
	gr_1v2 -> GetXaxis() -> SetRangeUser(0,300);

	gr_0v2 -> GetXaxis() -> SetLimits(0,300);
	gr_1v2 -> GetXaxis() -> SetLimits(0,300);

	gr_0v2 -> GetHistogram() -> SetMaximum(500);
	gr_0v2 -> GetHistogram() -> SetMinimum(240);
	gr_1v2 -> GetHistogram() -> SetMaximum(500);
	gr_1v2 -> GetHistogram() -> SetMinimum(240);

	

	gr_0v2 -> Draw("AP");
	gr_1v2 -> Draw("P SAME");

	if (DEBUG == true)	cout << "\nSCALER DATA" << endl;
	//----------------------------------------------------------------------
	// 						   scaler data
	//----------------------------------------------------------------------

	// doppie
	TCanvas* c_sca = new TCanvas("c_sca","c_sca",0,0,700,500);		
	TGraphErrors * gr_sca = new TGraphErrors;

	gr_sca -> GetXaxis() -> SetTitle("Ore passate");
	gr_sca -> GetYaxis() -> SetTitle("Conteggi");
	gr_sca -> SetTitle("Conteggi doppie");
	gr_sca -> SetMarkerStyle(20);
	gr_sca -> SetMarkerSize(0.5);
	gr_sca -> SetMarkerColor(kRed);

	gr_sca -> SetPoint(gr_sca -> GetN(), 0, 0);

	double yn = 0;

	for(int i = 0; i < doppie.size(); i++){
		double x = i;
		int N = gr_sca -> GetN();
		double y = (doppie[i] - yn) / 60;
		yn = doppie[i];
		gr_sca -> SetPoint(N,x,y);
	}

	c_sca -> cd();

	gr_sca -> Draw("AP");

	//singole
	TCanvas* c_scasing = new TCanvas("c_scasing","c_scasing",0,0,700,500);
	TGraphErrors * gr_sca0 = new TGraphErrors;
	TGraphErrors * gr_sca1 = new TGraphErrors;

	gr_sca0 -> GetXaxis() -> SetTitle("Ore passate");
	gr_sca0 -> GetYaxis() -> SetTitle("Conteggi");
	gr_sca0 -> SetTitle("Conteggi S1");
	gr_sca0 -> SetMarkerStyle(20);
	gr_sca0 -> SetMarkerSize(0.5);
	gr_sca0 -> SetMarkerColor(kRed);

	gr_sca1 -> GetXaxis() -> SetTitle("Ore passate");
	gr_sca1 -> GetYaxis() -> SetTitle("Conteggi");
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
		double y = (singola0[i] - yn0) / 60;
		yn0 = singola0[i];
		gr_sca0 -> SetPoint(N,x,y);
	}
	for(int i = 0; i < singola1.size(); i++){
		double x = i;
		int N = gr_sca1 -> GetN();
		double y = (singola1[i] - yn1) / 60;
		yn1 = singola1[i];
		gr_sca1 -> SetPoint(N,x,y);
	}

	c_scasing -> Divide(1,2);

	c_scasing -> cd(1);
	gr_sca0 -> Draw("AP");
	c_scasing -> cd(2);
	gr_sca1 -> Draw("AP");

	if (DEBUG == true)	cout << "\nISTOGRAMMA TDC" << endl;
	//----------------------------------------------------------------------
	// 						   istogramma tdc
	//----------------------------------------------------------------------

	TCanvas* c_tempi = new TCanvas("c_tempi","c_tempi",0,0,700,500);
	c_tempi -> Divide(2,1);

	double mint = 1000;
	double maxt = 0;

	for(int i = 0; i < tempi.size(); i++){
		if(tempi[i] < mint && tempi[i] != 0) mint = tempi[i];
		if(tempi[i] > maxt) maxt = tempi[i];
	}

	TH1D* th_tempi = new TH1D("Tempi", "Tempi", BINTEMPI, 0, 60);

	th_tempi -> GetXaxis() -> SetLimits(00, 60);
	th_tempi -> GetXaxis() -> SetRangeUser(0, 60);

	th_tempi -> SetFillColor(kCyan);

	for(int i = 0; i < tempi.size(); i++){//&& ampiezzas1[i] > mins1_lan && ampiezzas1[i] < maxs1_lan && ampiezzas2[i] > mins2_lan && ampiezzas2[i] < maxs2_lan
		if(tempi[i] != 0 && tempi[i] < 1000) {
			th_tempi -> Fill(tempi[i] * CONVERSIONETDC);
		}
	}

	c_tempi->cd(1);	

	c_tempi->Modified();	
	c_tempi->Update();	

	th_tempi -> Draw();

	//correzione
	TH1D* th_tempi_corr = new TH1D("Tempi corretti", "Tempi corretti", BINTEMPI, mint - 25, 1000);

	th_tempi_corr -> GetXaxis() -> SetLimits(mint - 20, 1000);
	th_tempi_corr -> GetXaxis() -> SetRangeUser(mint - 20, 1000);

	th_tempi_corr -> SetFillColor(kCyan);

	for(int i = 0; i < tempi.size(); i++){
		if(tempi[i] != 0 && tempi[i] < 1000){
			double x1 = ampiezzas1[i];
			double x2 = ampiezzas2[i];
			double adc0 = tau0 * log(x1/(x1-vth0));
			double adc1 = tau1 * log(x2/(x2-vth1));
			double x = tempi[i] - adc0 + adc1;
			th_tempi_corr -> Fill(x);
		}
	}

	c_tempi->cd(2);	

	c_tempi->Modified();	
	c_tempi->Update();	

	th_tempi_corr -> Draw();

	if (DEBUG == true)	cout << "\nSTIMA DI V" << endl;
	//----------------------------------------------------------------------
	// 						   stima velocità di m
	//----------------------------------------------------------------------

	TF1 * mygaus = new TF1("mygaus","gaus",85,95);	
	th_tempi_corr -> Fit("mygaus");
	double tmedio = mygaus -> GetParameter(1);
	double sigmat = mygaus -> GetParameter(2) * 0.119 /1000000000;
	tmedio = tmedio * CONVERSIONETDC - RITARDO;
	tmedio = tmedio / 1000000000;
	double c = dist / tmedio;
	double errc = sqrt(pow(E_DIST/tmedio,2)+pow(dist*sigmat/pow(tmedio,2),2));
	cout << "\n\n-------------------------------------------\n" << endl;
	cout << "stima della velocità dei muoni (m/s): " << c << " +- " << errc << endl;



	if (DEBUG == true)	cout << "\nISTOGRAMMA a 2 DIMENSIONI" << endl;
	//----------------------------------------------------------------------
	// 						        istogramma 2d
	//----------------------------------------------------------------------

	TCanvas* c_2d = new TCanvas("c_2d","c_2d",0,0,700,500);

	TH2D* th_3 = new TH2D("ADC S1-S2", "ADC S21-S", 200, 0, 200, 400, 0, 400);
	for(int i = 0; i < ampiezzas1.size(); i++){
		double x = ampiezzas1[i];
		double y = ampiezzas2[i];
		if(x!=0 && y!=0) th_3 -> Fill(x,y);
	}
	c_2d->cd();
	//th_3 -> Draw("surf1 [cutg]");
	th_3 -> Draw("cont4z");


	myApp.Run();
	return 0;
}


//cmake -DCMAKE_INSTALL_PREFIX=/home/rodolfo/GEANT/geant4.10.07 -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_OPENGL_X11=ON -DGEANT4_USE_GDML=ON  -DGEANT4_USE_XM=ON -DGEANT4_USE_QT=ON -DGEANT4_BUILD_MULTITHREADED=ON /home/rodolfo/GEANT/geant4.10.07 && make -j2 && make install 