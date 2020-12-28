//	c++ -o findbestlimits findbestlimits.cpp `root-config --cflags --glibs`
// ./findbestlimits /mnt/c/Users/Rodolfo/Desktop/calibrazione/3/ 

/*
*	Il programma serve a trovare i migliori parametri iniziale per il fit con l'inversa della sigmoide.
*
*
*
*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <random> 
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
#include "TVirtualFitter.h"
#include "TLegend.h"
#include "TLegendEntry.h"

using namespace std;

#define DEBUG false

#define GADC g_adc1

#define OFF_START 255
#define TAU_START -14.8
#define VTH_START 20.2

#define ADC0_FILE "datiADC0.txt"
#define ADC1_FILE "datiADC1.txt"
#define TDC_FILE "datiTDC.txt"

#define E_TDC 1.72
#define E_ADC 1


#define CHAN_ADC0 1
#define CHAN_ADC1 1
#define CHAN_TDC  1

#define BINTEMPI 100

#define DISTANZA 0.82
#define E_DIST 0.02

#define CONVERSIONETDC 0.1197 + 2.116
#define CONV_A 0.1197
#define CONV_B 2.116
#define ERR_CONV_A 0.000537
#define ERR_CONV_B 0.2389

#define RITARDO 30.6
#define E_RIT 2

class evento{
	public:
		evento();
		evento(double tdcval, double adc0val, double adc1val){
			tdc = tdcval;
			adc0 = adc0val;
			adc1 = adc1val;			
		};

		double Get_tdc(){
			return tdc;
		}
		double Get_adc0(){
			return adc0;
		}
		double Get_adc1(){
			return adc1;
		}
		
				//correzione nuova sigmoid
		double Get_tdc_corretto(double tau0, double vth0, double tau1, double vth1){
			return tdc + tau0 * log(vth0/(adc0-vth0))  - 4/tau0 - tau1 * log(vth1/(adc1-vth1)) + 4/tau1;
		}
		double Get_tdc_adc0(double tau0, double vth0, double tau1, double vth1){
			return tdc - tau0 * log(vth0/(adc0-vth0)) + 4/tau0;
		}
		double Get_tdc_adc1(double tau0, double vth0, double tau1, double vth1){
			return tdc - tau1 * log(vth1/(adc1-vth1)) + 4/tau1;
		}

		
	
		double Get_tdc_off(double off0, double off1){
			return - tdc + off0 + off1;
		}
	private:
		double tdc;
		double adc0;
		double adc1;
};

vector<evento*> v_eventi;

bool leggi_dati(vector<double> & ampiezzas1, int channel, char* myFile){
	double scal1;
	ifstream Infile;
	Infile.open(myFile, fstream::in);
	if (Infile.good () == false){
		cout << "Errore nell'apertura dei file " << myFile << endl;
		return false;
	}
	cout << "Leggo file di dati " << myFile << endl;
	while(true){
		Infile >> scal1;
		ampiezzas1.push_back(scal1);
		if (Infile.eof() == true) break;
		if (ampiezzas1.size()==300000) break;
	}
	Infile.close();
	cout << "Numero dati: " << ampiezzas1.size() << endl;
	return true;
}

		// funzione sigmoide
double myFun(double *x, double *par){		
	double A = par[0];		//tau
	double B = par[1];		//Vth
	double C = par[2];		//offset
	double D = x[0];
	return C+A*log(B/(D-B));
}




void graficoiniziale(double mins1_graph,double maxs1_graph,TGraphErrors* g_adc0,double mins2_graph,double maxs2_graph,TGraphErrors* g_adc1){
	for(int i = mins1_graph; i <= maxs1_graph; i++){
		double sum = 0;
		double sumt = 0;
		int count = 0;
		double dev = 0;
		for(int j = 0; j < v_eventi.size(); j++){
			double adc0 = v_eventi[j] -> Get_adc0();
			double tdc = v_eventi[j] -> Get_tdc();
			if(adc0 == i){
				count++;
				sumt = sumt + tdc;
			}
		}
		sumt = sumt / count;
		for(int j = 0; j < v_eventi.size(); j++){
			double adc0 = v_eventi[j] -> Get_adc0();
			double tdc = v_eventi[j] -> Get_tdc();
			if(adc0 == i){
				dev = dev + pow(tdc-sumt,2)/(count -1);
			}
		}
		dev = sqrt(dev) / sqrt(count);
		if(count == 1) dev = E_TDC;
		int N = g_adc0 -> GetN();
		if(count != 0) g_adc0 -> SetPoint(N,i,sumt);
		if(count != 0) g_adc0 -> SetPointError(N,E_ADC,dev);
	}

	for(int i = mins2_graph; i <= maxs2_graph; i++){
		double sum = 0;
		double sumt = 0;
		int count = 0;
		double dev = 0;
		for(int j = 0; j < v_eventi.size(); j++){
			double adc1 = v_eventi[j] -> Get_adc1();
			double tdc = v_eventi[j] -> Get_tdc();
			if(adc1 == i){
				count++;
				sumt = sumt + tdc;
			}
		}
		sumt = sumt / count;
		for(int j = 0; j < v_eventi.size(); j++){
			double adc1 = v_eventi[j] -> Get_adc1();
			double tdc = v_eventi[j] -> Get_tdc();
			if(adc1 == i){
				dev = dev + pow(tdc-sumt,2)/(count -1);
			}
		}
		dev = sqrt(dev) / sqrt(count);
		if(count == 1) dev = E_TDC;
		int N = g_adc1 -> GetN();
		if(count != 0) g_adc1 -> SetPoint(N,i,sumt);
		if(count != 0) g_adc1 -> SetPointError(N,E_ADC,dev);
	}
}

random_device rd; // obtain a random number from hardware
mt19937 gen(rd());
uniform_real_distribution<> random_gen(-1, 1000.1);

double rand_adc(double min, double max){
	double n;
	do{
		n = random_gen(gen);
	}
	while (n < min || n > max);
	return n;
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
	double dist = 0;

	vector<double> ampiezzas1;
	vector<double> ampiezzas2;
	vector<double> tempi;


	string path = argv[1];
	string path_adc0 = path + ADC0_FILE;
	string path_adc1 = path + ADC1_FILE;
	string path_tdc = path + TDC_FILE;

	if(!(leggi_dati(ampiezzas1, CHAN_ADC0, &path_adc0[0]))) return 1;
	if(!(leggi_dati(tempi, CHAN_TDC, &path_tdc[0]))) return 1;
	if(!(leggi_dati(ampiezzas2, CHAN_ADC1, &path_adc1[0]))) return 1;

	for(int i = 0; i < tempi.size(); i++){
		if(tempi[i] != 0 && ampiezzas1[i] != 0 && ampiezzas2[i] != 0){
			evento * temp = new evento(tempi[i], ampiezzas1[i], ampiezzas2[i]);
			v_eventi.push_back(temp);
		}
		else{
			dist++;
		}
	}

	cout << "Eventi: " << v_eventi.size() << endl;
	cout << "Eventi scartati: " << dist << endl;


	if(argc < 3) dist = DISTANZA;
	else dist = stod(argv[2]);


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

	TH1D* h_adc0 = new TH1D("ADC S1", "ADC S1", 1000, 0, 1000);	
	TH1D* h_adc1 = new TH1D("ADC S2", "ADC S2", 1000, 0, 1000);	
	TH2D* h2_adc01 = new TH2D("ADC S1-S2", "ADC S21-S", 100, 0, 200, 100, 0, 400);

	for(int i = 0; i < v_eventi.size(); i++){
		double adc0 = v_eventi[i] -> Get_adc0();
		double adc1 = v_eventi[i] -> Get_adc1();
		double tdc = v_eventi[i] -> Get_tdc();

		h_adc0 -> Fill(adc0);
		h_adc1 -> Fill(adc1);
		h2_adc01 -> Fill(adc0,adc1);
	}

	if (DEBUG == true)	cout << "\nGRAFICO INIZIALE" << endl;
	//----------------------------------------------------------------------
	// 						   grafico iniziale
	//----------------------------------------------------------------------

	TCanvas * myCanv_initial = new TCanvas("myCanv_initial","myCanv_initial",0,0,700,500);
	TGraphErrors * g_adc0 = new TGraphErrors;
	TGraphErrors * g_adc1 = new TGraphErrors;

	g_adc0 -> GetXaxis() -> SetTitle("canali ADC");
	g_adc0 -> GetYaxis() -> SetTitle("canali TDC");
	g_adc0 -> SetMarkerStyle(20);
	g_adc0 -> SetMarkerSize(0.5);
	g_adc0 -> SetMarkerColor(kRed);

	g_adc1 -> GetXaxis() -> SetTitle("canali ADC");
	g_adc1 -> GetYaxis() -> SetTitle("canali TDC");
	g_adc1 -> SetMarkerStyle(20);
	g_adc1 -> SetMarkerSize(0.5);
	g_adc1 -> SetMarkerColor(kBlue);

	//------------find limiti per i grafici
	double mins1_graph;
	double maxs1_graph;
	double mins2_graph;
	double maxs2_graph;

	mins1_graph = 0;
	maxs2_graph = 1000;
	mins2_graph = 0;
	maxs1_graph = 1000;
	
	graficoiniziale(mins1_graph,maxs1_graph,g_adc0,mins2_graph,maxs2_graph,g_adc1);

	g_adc0 -> Draw("A P");
	g_adc1 -> Draw("P same");

	
	if (DEBUG == true)	cout << "\nFIT" << endl;
	//----------------------------------------------------------------------
	// 						         fit
	//----------------------------------------------------------------------

	double tau = TAU_START;
	double vth = VTH_START;																														//inserire i parametri del fit
	double off = OFF_START;		
	
	double adc_min;
	double adc_max;

	double best_adc_min;
	double best_adc_max;

	double best_prob = 0;

	TF1 * myFun_golf = new TF1("myFun_golf",myFun, 0,300,3);
	myFun_golf -> SetLineColor(kRed);

	myFun_golf -> SetParName(0, "Tau0");
	myFun_golf -> SetParName(1, "Vth0");
	myFun_golf -> SetParName(2, "Off0");

	TCanvas* myCanvas_fit = new TCanvas("myCanvas_fit","myCanvas_fit",0,0,700,500);

	TGraphErrors * myGraph_fit_liv0 = new TGraphErrors;
	myGraph_fit_liv0 -> Draw("A P");
	TGraphErrors * myGraph_fit_liv1 = new TGraphErrors;
	myGraph_fit_liv1 -> Draw("same P");
	TGraphErrors * myGraph_fit_liv2 = new TGraphErrors;
	myGraph_fit_liv2 -> Draw("same P");
	TGraphErrors * myGraph_fit_liv3 = new TGraphErrors;
	myGraph_fit_liv3 -> Draw("same P");

	myGraph_fit_liv0 -> SetMarkerStyle(kCircle);
	myGraph_fit_liv1 -> SetMarkerStyle(kCircle);
	myGraph_fit_liv2 -> SetMarkerStyle(kCircle);
	myGraph_fit_liv3 -> SetMarkerStyle(kCircle);

	myGraph_fit_liv0 -> SetLineColor(kCyan);
	myGraph_fit_liv1 -> SetLineColor(kBlue);
	myGraph_fit_liv2 -> SetLineColor(kMagenta);
	myGraph_fit_liv3 -> SetLineColor(kRed);

	myGraph_fit_liv0 -> SetMarkerColor(kCyan);
	myGraph_fit_liv1 -> SetMarkerColor(kBlue);
	myGraph_fit_liv2 -> SetMarkerColor(kMagenta);
	myGraph_fit_liv3 -> SetMarkerColor(kRed);

	myGraph_fit_liv0 -> SetFillColor(kCyan);
	myGraph_fit_liv1 -> SetFillColor(kBlue);
	myGraph_fit_liv2 -> SetFillColor(kMagenta);
	myGraph_fit_liv3 -> SetFillColor(kRed);

	TLegend* legend_fit = new TLegend(.1,.7,.3,.9);
	TLegendEntry* entry_fit_0 = legend_fit -> AddEntry("myGraph_fit_liv0","Prob < 0.05");
	TLegendEntry* entry_fit_1 = legend_fit -> AddEntry("myGraph_fit_liv1","Prob < 0.20");
	TLegendEntry* entry_fit_2 = legend_fit -> AddEntry("myGraph_fit_liv2","Prob < 0.75");
	TLegendEntry* entry_fit_3 = legend_fit -> AddEntry("myGraph_fit_liv3","Prob < 1");

	entry_fit_0 -> SetFillColor(kCyan);
	entry_fit_1 -> SetFillColor(kBlue);
	entry_fit_2 -> SetFillColor(kMagenta);
	entry_fit_3 -> SetFillColor(kRed);

	legend_fit -> Draw("same");

	while(true == true){
		if(GADC == g_adc0){
			adc_min = rand_adc(mins1_graph,maxs1_graph);																	//funzione da scrivere
			adc_max = rand_adc(mins1_graph,maxs1_graph);
		}
		else if(GADC == g_adc1){
			adc_min = rand_adc(mins1_graph,maxs1_graph);																	//funzione da scrivere
			adc_max = rand_adc(mins1_graph,maxs1_graph);
		}

		if (adc_max < adc_min){
			double temp = adc_max;
			adc_max = adc_min;
			adc_min = temp;
		}
		if(adc_max - adc_min < 3){
			if(adc_max > 900) adc_min = adc_min - 3;
			else adc_max = adc_max + 3;
		}

		cout << "Tentativo: \t MIN = " << adc_min << " MAX = " << adc_max << endl;
		myFun_golf -> SetParameter(0, tau);
		myFun_golf -> SetParameter(1, vth);
		myFun_golf -> SetParameter(2, off);
				
		TFitResultPtr fit_result = GADC -> Fit("myFun_golf", "Q S R","", adc_min, adc_max); 

		int res = fit_result -> CovMatrixStatus();
		cout << res << endl;
		if (res != 0 && res != 2){
			double prob = fit_result -> Prob();
			if (prob > best_prob){
				best_prob = prob;
				best_adc_min = adc_min;
				best_adc_max = adc_max;

				cout << "\nProb = " << prob <<  
				"\n\t -> min = " << adc_min << " max = " << adc_max << endl;
			}

			int N;
			if (prob < 0.05){
				N = myGraph_fit_liv0 -> GetN();
				myGraph_fit_liv0 -> SetPoint(N,adc_min,adc_max);
			}
			else if(prob < 0.20){
				N = myGraph_fit_liv1 -> GetN();
				myGraph_fit_liv1 -> SetPoint(N,adc_min,adc_max);
			}
			else if (prob < 0.75){
				N = myGraph_fit_liv2 -> GetN();
				myGraph_fit_liv2 -> SetPoint(N,adc_min,adc_max);
			}
			else{
				N = myGraph_fit_liv3 -> GetN();
				myGraph_fit_liv3 -> SetPoint(N,adc_min,adc_max);
			}
						
		}
		myCanvas_fit -> Modified();
		myCanvas_fit -> Update();
		
	}
	

	myApp.Run();
	return 0;
}
