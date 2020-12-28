//	
// ./correzione_golf /mnt/c/Users/Rodolfo/Desktop/calibrazione/3/ 

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
#include "TVirtualFitter.h"

using namespace std;

#define DEBUG false

#define AUTOLIMIT true

#define ADC0_FILE "datiADC0.txt"
#define ADC1_FILE "datiADC1.txt"
#define TDC_FILE "datiTDC.txt"

#define E_TDC 1.72
#define E_ADC 1


#define MINS1 0
#define MAXS1 900
#define MINS2 0
#define MAXS2 900

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



	//setlimits 
bool fun1 = true;
void setlimits(TH1D * myhisto, double * min, double * max){	
	TF1 * g = new TF1("g","landau",85,95);	
	if(fun1 == true){
		g -> SetParameter (1,17);
	}
	else g -> SetParameter (1,50);
	myhisto -> Fit(g, "M Q");
	double mean = g -> GetParameter (1);
	double sigma = g -> GetParameter (2);
	if(fun1 == true){
		*min = mean - 1 * sigma;
		*max = mean + 3 * sigma;
		fun1 = false;
	}
	else{
		*min = mean - 1.5 * sigma;
		*max = mean + 2 * sigma;
	}
	cout << "media: " << mean << "  sigma: " << sigma << endl;
	return;
}

/*
	//setlimits con FWHM
void setlimits(TH1D * myhisto, double * min, double * max){
	double maxcount = 0;
	int maxcountbin = 0;
	for(int i = 0; i < 900; i++){
		int count = myhisto -> GetBinContent(i);
		if (count > maxcount){ 
			maxcountbin = i;
			maxcount = count;
		}
	}
	maxcount = maxcount/2;

	int minbin;
	int maxbin;

	double delta = 100000;
	for(int i = 0; i < maxcountbin; i++){
		double temp = abs(maxcount - myhisto -> GetBinContent(i));
		if(delta > temp){
			delta = temp;
			minbin = i;
		}
	}

	delta = 100000;
	for(int i = maxcountbin; i < 900 ; i++){
		double temp = abs(maxcount - myhisto -> GetBinContent(i));
		if(delta > temp){
			delta = temp;
			maxbin = i;
		}
	}

	*min = myhisto -> GetBinCenter(minbin);
	*max = myhisto -> GetBinCenter(maxbin);

	cout << "\nmaxcount: " << maxcount << endl;
	cout << "bin: " << minbin << " - " << maxcountbin << " - " << maxbin << endl;
}
*/

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

	TCanvas* c_graphv1 = new TCanvas("c_graphv1","c_graphv1",0,0,700,500);
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

	cout << "\n\nAUTOLIMIT: " << AUTOLIMIT << endl;
	if(AUTOLIMIT == true){
		setlimits(h_adc0, & mins1_graph, & maxs1_graph);
		setlimits(h_adc1, & mins2_graph, & maxs2_graph);
	}
	else{
		mins1_graph = MINS1;
		maxs2_graph = MAXS2;
		mins2_graph = MINS2;
		maxs1_graph = MAXS1;
	}
	cout << "S1: " << mins1_graph << " < x < " << maxs1_graph << endl;
	cout << "S2: " << mins2_graph << " < x < " << maxs2_graph << endl;
	cout << endl;

	graficoiniziale(mins1_graph,maxs1_graph,g_adc0,mins2_graph,maxs2_graph,g_adc1);

	c_graphv1 -> cd();

	g_adc0 -> GetXaxis() -> SetRangeUser(0,maxs2_graph + 20);
	g_adc1 -> GetXaxis() -> SetRangeUser(0,maxs2_graph + 20);

	g_adc0 -> GetXaxis() -> SetLimits(0,maxs2_graph + 20);
	g_adc1 -> GetXaxis() -> SetLimits(0,maxs2_graph + 20);

	g_adc0 -> GetHistogram() -> SetMaximum(300);
	g_adc0 -> GetHistogram() -> SetMinimum(200);
	g_adc1 -> GetHistogram() -> SetMaximum(300);
	g_adc1 -> GetHistogram() -> SetMinimum(200);

	g_adc0 -> Draw("AP z");
	g_adc1 -> Draw("P z same");

	if (DEBUG == true)	cout << "\nFIT" << endl;
	//----------------------------------------------------------------------
	// 						         fit
	//----------------------------------------------------------------------

	double tau;
	double vth;
	double off;		

	double start_tau = -20;
	double start_off = 10;
	double start_vth = 8;

	double step_tau = 5e3;
	double step_off = 1;
	double step_vth = 0.01;

	double max_tau = 0;
	double max_off = 400;
	double max_vth = 20;

	TF1 * myFun_golf = new TF1("myFun_golf",myFun, 0,300,3);
	myFun_golf -> SetLineColor(kRed);

	myFun_golf -> SetParName(0, "Tau0");
	myFun_golf -> SetParName(1, "Vth0");
	myFun_golf -> SetParName(2, "Off0");

	TCanvas* myCanvas_off = new TCanvas("myCanvas_off","myCanvas_off",0,0,700,500);
	TGraphErrors * myGraph_off = new TGraphErrors;
	myGraph_off -> Draw("A P");

	TCanvas* myCanvas_vth = new TCanvas("myCanvas_vth","myCanvas_vth",0,0,700,500);
	TGraphErrors * myGraph_vth = new TGraphErrors;
	myGraph_vth -> Draw("A P");

	TCanvas* myCanvas_tau = new TCanvas("myCanvas_tau","myCanvas_tau",0,0,700,500);
	TGraphErrors * myGraph_tau = new TGraphErrors;
	myGraph_tau -> Draw("A P");

	for(tau = start_tau; tau < max_tau; tau = tau + step_tau){
		for(vth = start_vth; vth < max_vth; vth = vth + step_vth){
			for(off = start_off; off < max_off; off = off + step_off){
				//cout << "tau = " << tau << " vth = " << vth << " off = " << off << " ";

				myFun_golf -> SetParameter(0, tau);
				myFun_golf -> SetParameter(1, vth);
				myFun_golf -> SetParameter(2, off);

				TFitResultPtr fit_result = g_adc1 -> Fit("myFun_golf", "Q S");

				double result_tau = myFun_golf -> GetParameter(0);
				double result_vth = myFun_golf -> GetParameter(1);
				double result_off = myFun_golf -> GetParameter(2);

				int res = fit_result -> CovMatrixStatus();
				//cout << res << endl;
				if (res != 0 && res != 2 && result_off > 0){
			//		cout << "ok" << endl;
					int N = myGraph_off -> GetN();
					myGraph_off -> SetPoint(N,off,result_off);

					N = myGraph_vth -> GetN();
					myGraph_vth -> SetPoint(N,vth,result_off);

					N = myGraph_tau -> GetN();
					myGraph_tau -> SetPoint(N,tau,result_off);
				}
			//	else cout << "no" << endl;

				myCanvas_off -> Modified();
				myCanvas_off -> Update();
			}
			myCanvas_vth -> Modified();
			myCanvas_vth -> Update();
		}
		myCanvas_tau -> Modified();
		myCanvas_tau -> Update();
	}
	

	myApp.Run();
	return 0;
}
