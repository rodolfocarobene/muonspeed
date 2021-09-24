//	c++ -o correzione_golf correzione_golf.cpp `root-config --cflags --glibs`
// ./correzione_golf /mnt/c/Users/Rodolfo/Desktop/calibrazione/3/ 

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

#include "libraryCoinc.h"

using namespace std;

#define DEBUG false

#define AUTOLIMIT true
#define RAND false

#define GADC g_adc1

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

vector<evento*> v_eventi;

	//setlimits con FWHM

random_device rd; // obtain a random number from hardware
mt19937 gen(rd());
uniform_real_distribution<> random_tau(-30, 0);
uniform_real_distribution<> random_vth(0, 60);

int main(int argc, char *argv[]){
	if (DEBUG == true)	cout << "\nLETTURA DEI FILE" << endl;
	//----------------------------------------------------------------------
	// 						    lettura dei file
	//----------------------------------------------------------------------

	vector<double> ampiezzas1;
	vector<double> ampiezzas2;
	vector<double> tempi;

	int dist = leggi_dati_container(ampiezzas1, ampiezzas2, tempi, argc, argv);

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

	TGraphErrors * g_adc0 = new TGraphErrors;
	TGraphErrors * g_adc1 = new TGraphErrors;

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

	
	if (DEBUG == true)	cout << "\nFIT" << endl;
	//----------------------------------------------------------------------
	// 						         fit
	//----------------------------------------------------------------------

	double tau;
	double vth;
	double off;		
	double start_tau = -16;
	double start_off = 10;
	double start_vth = 8;

	double step_tau = 2;
	double step_off = 1;
	double step_vth = 2;

	double max_tau = -9;
	double max_off = 400;
	double max_vth = 12;

	double best_tau;
	double best_off;
	double best_vth;

	double best_prob = 0;

	TF1 * myFun_golf = new TF1("myFun_golf",myFun, 0,300,3);
	myFun_golf -> SetLineColor(kRed);

	myFun_golf -> SetParName(0, "Tau0");
	myFun_golf -> SetParName(1, "Vth0");
	myFun_golf -> SetParName(2, "Off0");

	TCanvas* myCanvas_off = new TCanvas("myCanvas_off","myCanvas_off",0,0,700,500);

	TGraphErrors * myGraph_off_liv0 = new TGraphErrors;
	myGraph_off_liv0 -> Draw("A P");
	TGraphErrors * myGraph_off_liv1 = new TGraphErrors;
	myGraph_off_liv1 -> Draw("same P");
	TGraphErrors * myGraph_off_liv2 = new TGraphErrors;
	myGraph_off_liv2 -> Draw("same P");
	TGraphErrors * myGraph_off_liv3 = new TGraphErrors;
	myGraph_off_liv3 -> Draw("same P");

	myGraph_off_liv0 -> SetMarkerStyle(kCircle);
	myGraph_off_liv1 -> SetMarkerStyle(kCircle);
	myGraph_off_liv2 -> SetMarkerStyle(kCircle);
	myGraph_off_liv3 -> SetMarkerStyle(kCircle);

	myGraph_off_liv0 -> SetLineColor(kCyan);
	myGraph_off_liv1 -> SetLineColor(kBlue);
	myGraph_off_liv2 -> SetLineColor(kMagenta);
	myGraph_off_liv3 -> SetLineColor(kRed);

	myGraph_off_liv0 -> SetMarkerColor(kCyan);
	myGraph_off_liv1 -> SetMarkerColor(kBlue);
	myGraph_off_liv2 -> SetMarkerColor(kMagenta);
	myGraph_off_liv3 -> SetMarkerColor(kRed);

	myGraph_off_liv0 -> SetFillColor(kCyan);
	myGraph_off_liv1 -> SetFillColor(kBlue);
	myGraph_off_liv2 -> SetFillColor(kMagenta);
	myGraph_off_liv3 -> SetFillColor(kRed);

	TLegend* legend_off = new TLegend(.1,.7,.3,.9);
	TLegendEntry* entry_off_0 = legend_off -> AddEntry("myGraph_off_liv0","Prob < 0.05");
	TLegendEntry* entry_off_1 = legend_off -> AddEntry("myGraph_off_liv1","Prob < 0.20");
	TLegendEntry* entry_off_2 = legend_off -> AddEntry("myGraph_off_liv2","Prob < 0.75");
	TLegendEntry* entry_off_3 = legend_off -> AddEntry("myGraph_off_liv3","Prob < 1");

	entry_off_0 -> SetFillColor(kCyan);
	entry_off_1 -> SetFillColor(kBlue);
	entry_off_2 -> SetFillColor(kMagenta);
	entry_off_3 -> SetFillColor(kRed);

	legend_off -> Draw("same");


	TCanvas* myCanvas_vth = new TCanvas("myCanvas_vth","myCanvas_vth",0,0,700,500);
	TGraphErrors * myGraph_vth_0 = new TGraphErrors;
	myGraph_vth_0 -> Draw("A P");
	TGraphErrors * myGraph_vth_1 = new TGraphErrors;
	myGraph_vth_1 -> Draw("same P");
	TGraphErrors* myGraph_vth_2 = new TGraphErrors;
	myGraph_vth_2 -> Draw("same P");
	TGraphErrors * myGraph_vth_3 = new TGraphErrors;
	myGraph_vth_3 -> Draw("same P");

	myGraph_vth_0 -> SetMarkerStyle(kCircle);
	myGraph_vth_1 -> SetMarkerStyle(kCircle);
	myGraph_vth_2 -> SetMarkerStyle(kCircle);
	myGraph_vth_3 -> SetMarkerStyle(kCircle);

	myGraph_vth_0 -> SetLineColor(kCyan);
	myGraph_vth_1 -> SetLineColor(kBlue);
	myGraph_vth_2 -> SetLineColor(kMagenta);
	myGraph_vth_3 -> SetLineColor(kRed);

	myGraph_vth_0 -> SetMarkerColor(kCyan);
	myGraph_vth_1 -> SetMarkerColor(kBlue);
	myGraph_vth_2 -> SetMarkerColor(kMagenta);
	myGraph_vth_3 -> SetMarkerColor(kRed);

	myGraph_vth_0 -> SetFillColor(kCyan);
	myGraph_vth_1 -> SetFillColor(kBlue);
	myGraph_vth_2 -> SetFillColor(kMagenta);
	myGraph_vth_3 -> SetFillColor(kRed);

	TLegend* legend_vth = new TLegend(.1,.7,.3,.9);
	TLegendEntry* entry_vth_0 = legend_vth -> AddEntry("myGraph_vth_0","Prob < 0.05");
	TLegendEntry* entry_vth_1 = legend_vth -> AddEntry("myGraph_vth_1","Prob < 0.20");
	TLegendEntry* entry_vth_2 = legend_vth -> AddEntry("myGraph_vth_2","Prob < 0.75");
	TLegendEntry* entry_vth_3 = legend_vth -> AddEntry("myGraph_vth_3","Prob < 1");

	entry_vth_0 -> SetFillColor(kCyan);
	entry_vth_1 -> SetFillColor(kBlue);
	entry_vth_2 -> SetFillColor(kMagenta);
	entry_vth_3 -> SetFillColor(kRed);

	legend_vth -> Draw("same");

	TCanvas* myCanvas_tau = new TCanvas("myCanvas_tau","myCanvas_tau",0,0,700,500);
	TGraphErrors * myGraph_tau_0 = new TGraphErrors;
	myGraph_tau_0 -> Draw("A P");
	TGraphErrors * myGraph_tau_1 = new TGraphErrors;
	myGraph_tau_1 -> Draw("same P");
	TGraphErrors * myGraph_tau_2 = new TGraphErrors;
	myGraph_tau_2 -> Draw("same P");
	TGraphErrors * myGraph_tau_3 = new TGraphErrors;
	myGraph_tau_3 -> Draw("same P");

	myGraph_tau_0 -> SetMarkerStyle(kCircle);
	myGraph_tau_1 -> SetMarkerStyle(kCircle);
	myGraph_tau_2 -> SetMarkerStyle(kCircle);
	myGraph_tau_3 -> SetMarkerStyle(kCircle);

	myGraph_tau_0 -> SetLineColor(kCyan);
	myGraph_tau_1 -> SetLineColor(kBlue);
	myGraph_tau_2 -> SetLineColor(kMagenta);
	myGraph_tau_3 -> SetLineColor(kRed);

	myGraph_tau_0 -> SetMarkerColor(kCyan);
	myGraph_tau_1 -> SetMarkerColor(kBlue);
	myGraph_tau_2 -> SetMarkerColor(kMagenta);
	myGraph_tau_3 -> SetMarkerColor(kRed);

	myGraph_tau_0 -> SetFillColor(kCyan);
	myGraph_tau_1 -> SetFillColor(kBlue);
	myGraph_tau_2 -> SetFillColor(kMagenta);
	myGraph_tau_3 -> SetFillColor(kRed);

	TLegend* legend_tau = new TLegend(.1,.7,.3,.9);
	TLegendEntry* entry_tau_0 = legend_tau -> AddEntry("myGraph_tau_0","Prob < 0.05");
	TLegendEntry* entry_tau_1 = legend_tau -> AddEntry("myGraph_tau_1","Prob < 0.20");
	TLegendEntry* entry_tau_2 = legend_tau -> AddEntry("myGraph_tau_2","Prob < 0.75");
	TLegendEntry* entry_tau_3 = legend_tau -> AddEntry("myGraph_tau_3","Prob < 1");

	entry_tau_0 -> SetFillColor(kCyan);
	entry_tau_1 -> SetFillColor(kBlue);
	entry_tau_2 -> SetFillColor(kMagenta);
	entry_tau_3 -> SetFillColor(kRed);

	legend_tau -> Draw("same");

	TCanvas* auto_myCanvas_tau = new TCanvas("auto_myCanvas_tau","auto_myCanvas_tau",0,0,700,500);
	TGraphErrors * auto_myGraph_tau_0 = new TGraphErrors;
	auto_myGraph_tau_0 -> Draw("A P");
	TGraphErrors * auto_myGraph_tau_1 = new TGraphErrors;
	auto_myGraph_tau_1 -> Draw("same P");
	TGraphErrors * auto_myGraph_tau_2 = new TGraphErrors;
	auto_myGraph_tau_2 -> Draw("same P");
	TGraphErrors * auto_myGraph_tau_3 = new TGraphErrors;
	auto_myGraph_tau_3 -> Draw("same P");

	auto_myGraph_tau_0 -> SetMarkerStyle(kCircle);
	auto_myGraph_tau_1 -> SetMarkerStyle(kCircle);
	auto_myGraph_tau_2 -> SetMarkerStyle(kCircle);
	auto_myGraph_tau_3 -> SetMarkerStyle(kCircle);

	auto_myGraph_tau_0 -> SetLineColor(kCyan);
	auto_myGraph_tau_1 -> SetLineColor(kBlue);
	auto_myGraph_tau_2 -> SetLineColor(kMagenta);
	auto_myGraph_tau_3 -> SetLineColor(kRed);

	auto_myGraph_tau_0 -> SetMarkerColor(kCyan);
	auto_myGraph_tau_1 -> SetMarkerColor(kBlue);
	auto_myGraph_tau_2 -> SetMarkerColor(kMagenta);
	auto_myGraph_tau_3 -> SetMarkerColor(kRed);

	auto_myGraph_tau_0 -> SetFillColor(kCyan);
	auto_myGraph_tau_1 -> SetFillColor(kBlue);
	auto_myGraph_tau_2 -> SetFillColor(kMagenta);
	auto_myGraph_tau_3 -> SetFillColor(kRed);

	TLegend* auto_legend_tau = new TLegend(.1,.7,.3,.9);
	TLegendEntry* auto_entry_tau_0 = auto_legend_tau -> AddEntry("auto_myGraph_tau_0","Prob < 0.05");
	TLegendEntry* auto_entry_tau_1 = auto_legend_tau -> AddEntry("auto_myGraph_tau_1","Prob < 0.20");
	TLegendEntry* auto_entry_tau_2 = auto_legend_tau -> AddEntry("auto_myGraph_tau_2","Prob < 0.75");
	TLegendEntry* auto_entry_tau_3 = auto_legend_tau -> AddEntry("auto_myGraph_tau_3","Prob < 1");

	auto_entry_tau_0 -> SetFillColor(kCyan);
	auto_entry_tau_1 -> SetFillColor(kBlue);
	auto_entry_tau_2 -> SetFillColor(kMagenta);
	auto_entry_tau_3 -> SetFillColor(kRed);

	auto_legend_tau -> Draw("same");

	TCanvas* auto_myCanvas_vth = new TCanvas("auto_myCanvas_vth","auto_myCanvas_vth",0,0,700,500);
	TGraphErrors * auto_myGraph_vth_0 = new TGraphErrors;
	auto_myGraph_vth_0 -> Draw("A P");
	TGraphErrors * auto_myGraph_vth_1 = new TGraphErrors;
	auto_myGraph_vth_1 -> Draw("same P");
	TGraphErrors * auto_myGraph_vth_2 = new TGraphErrors;
	auto_myGraph_vth_2 -> Draw("same P");
	TGraphErrors * auto_myGraph_vth_3 = new TGraphErrors;
	auto_myGraph_vth_3 -> Draw("same P");

	auto_myGraph_vth_0 -> SetMarkerStyle(kCircle);
	auto_myGraph_vth_1 -> SetMarkerStyle(kCircle);
	auto_myGraph_vth_2 -> SetMarkerStyle(kCircle);
	auto_myGraph_vth_3 -> SetMarkerStyle(kCircle);

	auto_myGraph_vth_0 -> SetLineColor(kCyan);
	auto_myGraph_vth_1 -> SetLineColor(kBlue);
	auto_myGraph_vth_2 -> SetLineColor(kMagenta);
	auto_myGraph_vth_3 -> SetLineColor(kRed);

	auto_myGraph_vth_0 -> SetMarkerColor(kCyan);
	auto_myGraph_vth_1 -> SetMarkerColor(kBlue);
	auto_myGraph_vth_2 -> SetMarkerColor(kMagenta);
	auto_myGraph_vth_3 -> SetMarkerColor(kRed);

	auto_myGraph_vth_0 -> SetFillColor(kCyan);
	auto_myGraph_vth_1 -> SetFillColor(kBlue);
	auto_myGraph_vth_2 -> SetFillColor(kMagenta);
	auto_myGraph_vth_3 -> SetFillColor(kRed);

	TLegend* auto_legend_vth = new TLegend(.1,.7,.3,.9);
	TLegendEntry* auto_entry_vth_0 = auto_legend_vth -> AddEntry("auto_myGraph_vth_0","Prob < 0.05");
	TLegendEntry* auto_entry_vth_1 = auto_legend_vth -> AddEntry("auto_myGraph_vth_1","Prob < 0.20");
	TLegendEntry* auto_entry_vth_2 = auto_legend_vth -> AddEntry("auto_myGraph_vth_2","Prob < 0.75");
	TLegendEntry* auto_entry_vth_3 = auto_legend_vth -> AddEntry("auto_myGraph_vth_3","Prob < 1");

	auto_entry_vth_0 -> SetFillColor(kCyan);
	auto_entry_vth_1 -> SetFillColor(kBlue);
	auto_entry_vth_2 -> SetFillColor(kMagenta);
	auto_entry_vth_3 -> SetFillColor(kRed);

	auto_legend_vth -> Draw("same");

	if (RAND == false){
		for(tau = start_tau; tau < max_tau; tau = tau + step_tau){
			for(vth = start_vth; vth < max_vth; vth = vth + step_vth){
				cout << "Tentativo: \tTau = " << tau << " Vth = " << vth << endl;
				for(off = start_off; off < max_off; off = off + step_off){

					myFun_golf -> SetParameter(0, tau);
					myFun_golf -> SetParameter(1, vth);
					myFun_golf -> SetParameter(2, off);

					TFitResultPtr fit_result = GADC -> Fit("myFun_golf", "Q S");

					double result_tau = myFun_golf -> GetParameter(0);
					double result_vth = myFun_golf -> GetParameter(1);
					double result_off = myFun_golf -> GetParameter(2);

					int res = fit_result -> CovMatrixStatus();

					if (res != 0 && res != 2 && result_off > 0){
						double prob = fit_result -> Prob();
						cout << "Prob = " << prob << "\t Tau = " << tau << " Vth = " << vth << " Off = " << off << " -> " << result_off << endl;
						if (prob > best_prob){
							best_prob = prob;
							best_tau = tau;
							best_vth = vth;
							best_off = off;

							cout << "BEST -- Prob = " << prob << "\t Tau = " << tau << " Vth = " << vth << " Off = " << off << " -> " << result_off << endl;
						}

						int N;
						if (prob < 0.05){
							N = myGraph_off_liv0 -> GetN();
							myGraph_off_liv0 -> SetPoint(N,off,result_off);

							N = myGraph_vth_0 -> GetN();
							myGraph_vth_0 -> SetPoint(N,vth,result_off);

							N = myGraph_tau_0 -> GetN();
							myGraph_tau_0 -> SetPoint(N,tau,result_off);

							N = auto_myGraph_vth_0 -> GetN();
							auto_myGraph_vth_0 -> SetPoint(N,vth,result_vth);

							N = auto_myGraph_tau_0 -> GetN();
							auto_myGraph_tau_0 -> SetPoint(N,tau,result_tau);
						}
						else if(prob < 0.20){
							N = myGraph_off_liv1 -> GetN();
							myGraph_off_liv1 -> SetPoint(N,off,result_off);

							N = myGraph_vth_1 -> GetN();
							myGraph_vth_1 -> SetPoint(N,vth,result_off);

							N = myGraph_tau_1 -> GetN();
							myGraph_tau_1 -> SetPoint(N,tau,result_off);

							N = auto_myGraph_vth_1 -> GetN();
							auto_myGraph_vth_1 -> SetPoint(N,vth,result_vth);

							N = auto_myGraph_tau_1 -> GetN();
							auto_myGraph_tau_1 -> SetPoint(N,tau,result_tau);
						}
						else if (prob < 0.75){
							N = myGraph_off_liv2 -> GetN();
							myGraph_off_liv2 -> SetPoint(N,off,result_off);

							N = myGraph_vth_2 -> GetN();
							myGraph_vth_2 -> SetPoint(N,vth,result_off);

							N = myGraph_tau_2 -> GetN();
							myGraph_tau_2 -> SetPoint(N,tau,result_off);

							N = auto_myGraph_vth_2 -> GetN();
							auto_myGraph_vth_2 -> SetPoint(N,vth,result_vth);

							N = auto_myGraph_tau_2 -> GetN();
							auto_myGraph_tau_2 -> SetPoint(N,tau,result_tau);
						}
						else{
							N = myGraph_off_liv3 -> GetN();
							myGraph_off_liv3 -> SetPoint(N,off,result_off);

							N = myGraph_vth_3 -> GetN();
							myGraph_vth_3 -> SetPoint(N,vth,result_off);

							N = myGraph_tau_3 -> GetN();
							myGraph_tau_3 -> SetPoint(N,tau,result_off);

							N = auto_myGraph_vth_3 -> GetN();
							auto_myGraph_vth_3 -> SetPoint(N,vth,result_vth);

							N = auto_myGraph_tau_3 -> GetN();
							auto_myGraph_tau_3 -> SetPoint(N,tau,result_tau);
						}
						
					}
				

					myCanvas_off -> Modified();
					myCanvas_off -> Update();
					myCanvas_vth -> Modified();
					myCanvas_vth -> Update();
					myCanvas_tau -> Modified();
					myCanvas_tau -> Update();

					auto_myCanvas_vth -> Modified();
					auto_myCanvas_vth -> Update();
					auto_myCanvas_tau -> Modified();
					auto_myCanvas_tau -> Update();
				}
			}
		}
	}
	else{
		for(int times = 0; times < 1000; times++){
			tau = rand_tau();
			vth = rand_vth();

			cout << "Tentativo: \tTau = " << tau << " Vth = " << vth << endl;
			for(off = start_off; off < max_off; off = off + step_off){
				myFun_golf -> SetParameter(0, tau);
				myFun_golf -> SetParameter(1, vth);
				myFun_golf -> SetParameter(2, off);
				TFitResultPtr fit_result = GADC -> Fit("myFun_golf", "Q S"); 

				double result_tau = myFun_golf -> GetParameter(0);
				double result_vth = myFun_golf -> GetParameter(1);
				double result_off = myFun_golf -> GetParameter(2);

				int res = fit_result -> CovMatrixStatus();
				if (res != 0 && res != 2 && result_off > 0){
					double prob = fit_result -> Prob();
					cout << "Prob = " << prob << "\t Tau = " << tau << " Vth = " << vth << " Off = " << off << endl;
					if (prob > best_prob){
						best_prob = prob;
						best_tau = tau;
						best_vth = vth;
						best_off = off;

						cout << "Best Prob = " << prob << "\t Tau = " << tau << " Vth = " << vth << " Off = " << off << 
						"\n\t -> off = " << result_off << " tau = " << result_tau << " vth = " << result_vth << endl;
					}

					int N;
					if (prob < 0.05){
						N = myGraph_off_liv0 -> GetN();
						myGraph_off_liv0 -> SetPoint(N,off,result_off);

						N = myGraph_vth_0 -> GetN();
						myGraph_vth_0 -> SetPoint(N,vth,result_off);

						N = myGraph_tau_0 -> GetN();
						myGraph_tau_0 -> SetPoint(N,tau,result_off);

						N = auto_myGraph_vth_0 -> GetN();
						auto_myGraph_vth_0 -> SetPoint(N,vth,result_vth);

						N = auto_myGraph_tau_0 -> GetN();
						auto_myGraph_tau_0 -> SetPoint(N,tau,result_tau);
					}
					else if(prob < 0.20){
						N = myGraph_off_liv1 -> GetN();
						myGraph_off_liv1 -> SetPoint(N,off,result_off);

						N = myGraph_vth_1 -> GetN();
						myGraph_vth_1 -> SetPoint(N,vth,result_off);

						N = myGraph_tau_1 -> GetN();
						myGraph_tau_1 -> SetPoint(N,tau,result_off);

						N = auto_myGraph_vth_1 -> GetN();
						auto_myGraph_vth_1 -> SetPoint(N,vth,result_vth);

						N = auto_myGraph_tau_1 -> GetN();
						auto_myGraph_tau_1 -> SetPoint(N,tau,result_tau);
					}
					else if (prob < 0.75){
						N = myGraph_off_liv2 -> GetN();
						myGraph_off_liv2 -> SetPoint(N,off,result_off);

						N = myGraph_vth_2 -> GetN();
						myGraph_vth_2 -> SetPoint(N,vth,result_off);

						N = myGraph_tau_2 -> GetN();
						myGraph_tau_2 -> SetPoint(N,tau,result_off);

						N = auto_myGraph_vth_2 -> GetN();
						auto_myGraph_vth_2 -> SetPoint(N,vth,result_vth);

						N = auto_myGraph_tau_2 -> GetN();
						auto_myGraph_tau_2 -> SetPoint(N,tau,result_tau);
					}
					else{
						N = myGraph_off_liv3 -> GetN();
						myGraph_off_liv3 -> SetPoint(N,off,result_off);

						N = myGraph_vth_3 -> GetN();
						myGraph_vth_3 -> SetPoint(N,vth,result_off);

						N = myGraph_tau_3 -> GetN();
						myGraph_tau_3 -> SetPoint(N,tau,result_off);

						N = auto_myGraph_vth_3 -> GetN();
						auto_myGraph_vth_3 -> SetPoint(N,vth,result_vth);

						N = auto_myGraph_tau_3 -> GetN();
						auto_myGraph_tau_3 -> SetPoint(N,tau,result_tau);
					}
						
				}
					

				myCanvas_off -> Modified();
				myCanvas_off -> Update();
				myCanvas_vth -> Modified();
				myCanvas_vth -> Update();
				myCanvas_tau -> Modified();
				myCanvas_tau -> Update();

				auto_myCanvas_vth -> Modified();
				auto_myCanvas_vth -> Update();
				auto_myCanvas_tau -> Modified();
				auto_myCanvas_tau -> Update();
			}
		}
	}
	

	myApp.Run();
	return 0;
}
