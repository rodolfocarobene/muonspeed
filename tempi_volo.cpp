//c++ -o tempi_volo tempi_volo.cpp `root-config --cflags --glibs`
// ./tempi_volo /mnt/c/Users/Rodolfo/Desktop/calibrazione/3/ 

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
		
				//std correzione
		double Get_tdc_corretto(double tau0, double vth0, double tau1, double vth1){
			return tdc + tau0 * log(adc0/(adc0 - vth0)) - tau1 * log(adc1/(adc1 - vth1));
		}
		double Get_tdc_adc0(double tau0, double vth0, double tau1, double vth1){
			return tdc - tau0 * log(adc0/(adc0 - vth0));
		}
		double Get_tdc_adc1(double tau0, double vth0, double tau1, double vth1){
			return tdc - tau1 * log(adc1/(adc1 - vth1));
		}
		
		/*
				//correzione nuova log(log)
		double Get_tdc_corretto(double tau0, double vth0, double tau1, double vth1){
			return tdc - tau0 * log(log(adc0/vth0)) + tau1 * log(log(adc1/vth1));
		}
		double Get_tdc_adc0(double tau0, double vth0, double tau1, double vth1){
			return tdc + tau0 * log(log(adc0/vth0));
		}
		double Get_tdc_adc1(double tau0, double vth0, double tau1, double vth1){
			return tdc + tau1 * log(log(adc1/vth1));
		}

		*/
		/*
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

		*/
	
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
	double scal1, scal2, scal3, scal4, scal5, scal6, scal7, scal8;
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

bool leggi_dati2(vector<double> & ampiezzas1, int channel, char* myFile){
	double scal1, scal2, scal3, scal4, scal5, scal6, scal7, scal8;
	ifstream Infile;
	Infile.open(myFile, fstream::in);
	if (Infile.good () == false){
		return false;
	}
	cout << "Leggo file di dati " << myFile << endl;
	while(true){
		Infile >> scal1;
		ampiezzas1.push_back(scal1);
		if (Infile.eof() == true) break;
	}
	Infile.close();
	cout << "Numero dati: " << ampiezzas1.size() << endl;
	return true;
}


		//	funzione standard
double myFun(double *x, double *par){		
	double A = par[0];		//tau
	double B = par[1];		//Vth
	double C = par[2];		//offset
	double D = x[0];
	return A*log(D/(D-B))+C;
}


/*
		//	funzione nuova loglog
double myFun(double *x, double *par){		
	double A = par[0];		//tau
	double B = par[1];		//Vth
	double C = par[2];		//offset
	double D = x[0];
	return C-A*log(log(D/B));
}
*/
/*
		// funzione sigmoide
double myFun(double *x, double *par){		
	double A = par[0];		//tau
	double B = par[1];		//Vth
	double C = par[2];		//offset
	double D = x[0];
	return C+A*log(B/(D-B));
}
*/
/*
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
*/
	//setlimits con FWHM
void setlimits(TH1D * myhisto, double * min, double * max){
	double maxcount = 0;
	int maxcountbin = 0;
	for(int i = 0; i < 900; i++){
		int count = myhisto -> GetBinContent(i);
		if (count > maxcount) maxcountbin = i;
	}
	maxcount = maxcount/2;

	int minbin;
	int maxbin;

	double delta = 100000;
	for(int i = 0; i < maxcountbin; i++){
		double temp = maxcount - myhisto -> GetBinContent(i);
		if(delta > temp){
			delta = temp;
			minbin = i;
		}
	}

	delta = 100000;
	for(int i = maxcountbin; i < 900 ; i++){
		double temp = maxcount - myhisto -> GetBinContent(i);
		if(delta > temp){
			delta = temp;
			maxbin = i;
		}
	}

	*min = myhisto -> GetBinCenter(minbin);
	*max = myhisto -> GetBinCenter(maxbin);

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
	if(!(leggi_dati2(tempi, CHAN_TDC, &path_tdc[0]))) return 1;
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

	TCanvas* c_adc0 = new TCanvas("c_adc0","c_adc0",0,0,700,500);
	TCanvas* c_adc1 = new TCanvas("c_adc1","c_adc1",0,0,700,500);
	TCanvas* c_2d = new TCanvas("c_2d","c_2d",0,0,700,500);

	TH1D* h_adc0 = new TH1D("ADC S1", "ADC S1", 1000, 0, 1000);	
	TH1D* h_adc1 = new TH1D("ADC S2", "ADC S2", 1000, 0, 1000);	
	TH2D* h2_adc01 = new TH2D("ADC S1-S2", "ADC S21-S", 200, 0, 200, 400, 0, 400);

	for(int i = 0; i < v_eventi.size(); i++){
		double adc0 = v_eventi[i] -> Get_adc0();
		double adc1 = v_eventi[i] -> Get_adc1();
		double tdc = v_eventi[i] -> Get_tdc();

		h_adc0 -> Fill(adc0);
		h_adc1 -> Fill(adc1);
		h2_adc01 -> Fill(adc0,adc1);
	}

	c_adc0 -> cd();
	h_adc0 -> SetFillColor(kCyan);
	h_adc0 -> Draw();

	c_adc1 -> cd();
	h_adc1 -> SetFillColor(kCyan);
	h_adc1 -> Draw();

	c_2d -> cd();
	h2_adc01 -> Draw("cont4z");

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

	g_adc0 -> GetXaxis() -> SetRangeUser(0,300);
	g_adc1 -> GetXaxis() -> SetRangeUser(0,300);

	g_adc0 -> GetXaxis() -> SetLimits(0,300);
	g_adc1 -> GetXaxis() -> SetLimits(0,300);

	g_adc0 -> GetHistogram() -> SetMaximum(340);
	g_adc0 -> GetHistogram() -> SetMinimum(140);
	g_adc1 -> GetHistogram() -> SetMaximum(340);
	g_adc1 -> GetHistogram() -> SetMinimum(140);

	g_adc0 -> Draw("AP z");
	g_adc1 -> Draw("P z same");

	if (DEBUG == true)	cout << "\nFIT" << endl;
	//----------------------------------------------------------------------
	// 						         fit
	//----------------------------------------------------------------------

	TF1 * fun_adc0 = new TF1("fun_adc0",myFun, mins1_graph,maxs1_graph,3);
	fun_adc0 -> SetLineColor(kRed);
	TF1 * fun_adc1 = new TF1("fun_adc1",myFun, mins2_graph,maxs2_graph,3);
	fun_adc1 -> SetLineColor(kBlue);

	fun_adc0 -> SetParName(0, "Tau0");
	fun_adc0 -> SetParName(1, "Vth0");
	fun_adc0 -> SetParName(2, "Off0");

	fun_adc1 -> SetParName(0, "Tau1");
	fun_adc1 -> SetParName(1, "Vth1");
	fun_adc1 -> SetParName(2, "Off1");


	fun_adc0 -> SetParameter(0, -4e4);
	fun_adc0 -> SetParameter(1, 1e-2);
	fun_adc0 -> SetParameter(2, 280);

	fun_adc1 -> SetParameter(0, -4e4);
	fun_adc1 -> SetParameter(1, 1e-2);
	fun_adc1 -> SetParameter(2, 280);


	/*
			//sigmoide
	fun_adc0 -> SetParameter(0, -14.49);
	fun_adc0 -> SetParameter(1, 0.00037);
	fun_adc0 -> SetParameter(2, 93);

	fun_adc1 -> SetParameter(0, -15);
	fun_adc1 -> SetParameter(1, 0.001);
	fun_adc1 -> SetParameter(2, 90);
*/

/*			loglog
	fun_adc0 -> SetParameter(0, -400);
	fun_adc0 -> SetParameter(1, 1e-24);
	fun_adc0 -> SetParameter(2, -1000);

	fun_adc1 -> SetParameter(0, -400);
	fun_adc1 -> SetParameter(1, 1e-24);
	fun_adc1 -> SetParameter(2, -1000);
*/

	TVirtualFitter::SetMaxIterations(20000);

	TFitResultPtr r_0 = g_adc0 -> Fit("fun_adc0", "R S E M");
	TFitResultPtr r_1 = g_adc1 -> Fit("fun_adc1", "R S E M");

	c_graphv1 -> Modified();
	c_graphv1 -> Update();
	TPaveStats *st0 = (TPaveStats*) g_adc0->FindObject("stats");

	if (st0 == NULL) {
		cout << "st0 is nullptr!!!!" << endl;
	} else {
		st0 -> SetX1NDC(0.3); //new x start position
		st0 -> SetX2NDC(0.6); //new x end position
		c_graphv1 -> Modified();
		c_graphv1 -> Update();
	}

	c_graphv1 -> Modified();
	c_graphv1 -> Update();


	if (DEBUG == true)	cout << "\nGRAFICO CORRETTO" << endl;
	//----------------------------------------------------------------------
	// 						   grafico corretto
	//----------------------------------------------------------------------

	TCanvas* c_corretto = new TCanvas("c_corretto","c_corretto",0,0,700,500);
	TGraphErrors * g_adc0v2 = new TGraphErrors;
	TGraphErrors * g_adc1v2 = new TGraphErrors;

	g_adc0v2 -> GetXaxis() -> SetTitle("canali ADC");
	g_adc0v2 -> GetYaxis() -> SetTitle("canali TDC");
	g_adc0v2 -> SetMarkerStyle(20);
	g_adc0v2 -> SetMarkerSize(0.5);
	g_adc0v2 -> SetMarkerColor(kRed);

	g_adc1v2 -> GetXaxis() -> SetTitle("canali ADC");
	g_adc1v2 -> GetYaxis() -> SetTitle("canali TDC");
	g_adc1v2 -> SetMarkerStyle(20);
	g_adc1v2 -> SetMarkerSize(0.5);
	g_adc1v2 -> SetMarkerColor(kBlue);


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

	cout << "cov0 " << cov_tv_0 << endl;
	cout << "cov1 " << cov_tv_1 << endl;

	cout << "Delta offset: " << off1 - off0 << " -> "  << (off1 - off0) * CONVERSIONETDC << endl;

	for(int i = mins1_graph; i <= maxs1_graph; i++){
		double sum = 0;
		double sumt = 0;
		int count = 0;
		double dev = 1;
		for(int j = 0; j < v_eventi.size(); j++){
			double adc0 = v_eventi[j] -> Get_adc0();
			double tdc = v_eventi[j] -> Get_tdc_adc0(tau0,vth0,tau1,vth1);//Get_tdc_adc0(tau0,vth0,tau1,vth1);//Get_tdc_corretto(tau0,vth0,tau1,vth1);
			if(adc0 == i){
				count++;
				sumt = sumt + tdc;
			}
		}
		sumt = sumt / count;
		if(count == 1) dev = E_TDC;
		int N = g_adc0v2 -> GetN();
		if(count > 0) g_adc0v2 -> SetPoint(N,i,sumt);
		if(count > 0) g_adc0v2 -> SetPointError(N,E_ADC,dev);
		//cout << "punto: " << N << " " << i << " " << sumt << endl;
	}

	for(int i = mins2_graph; i <= maxs2_graph; i++){
		double sum = 0;
		double sumt = 0;
		int count = 0;
		double dev = 1;
		for(int j = 0; j < v_eventi.size(); j++){
			double adc1 = v_eventi[j] -> Get_adc1();
			double tdc = v_eventi[j] -> Get_tdc_adc1(tau0,vth0,tau1,vth1);//Get_tdc_adc1(tau0,vth0,tau1,vth1);//Get_tdc_corretto(tau0,vth0,tau1,vth1);
			if(adc1 == i){
				count++;
				sumt = sumt + tdc;
			}
		}
		sumt = sumt / count;
		if(count == 1) dev = E_TDC;
		int N = g_adc1v2 -> GetN();
		if(count > 0) g_adc1v2 -> SetPoint(N,i,sumt);
		if(count > 0) g_adc1v2 -> SetPointError(N,E_ADC,dev);
	}


	g_adc0v2 -> GetXaxis() -> SetRangeUser(0,300);
	g_adc1v2 -> GetXaxis() -> SetRangeUser(0,300);

	g_adc0v2 -> GetXaxis() -> SetLimits(0,300);
	g_adc1v2 -> GetXaxis() -> SetLimits(0,300);

	g_adc0v2 -> GetHistogram() -> SetMaximum(600);
	g_adc0v2 -> GetHistogram() -> SetMinimum(100);
	g_adc1v2 -> GetHistogram() -> SetMaximum(600);
	g_adc1v2 -> GetHistogram() -> SetMinimum(100);

	g_adc0v2 -> Draw("AP z");
	g_adc1v2 -> Draw("P z same");
	
	
	if (DEBUG == true)	cout << "\nISTOGRAMMA TDC" << endl;
	//----------------------------------------------------------------------
	// 						   istogramma tdc
	//----------------------------------------------------------------------

	TCanvas* c_tempi = new TCanvas("c_tempi","c_tempi",0,0,700,500);
	c_tempi -> Divide(2,1);


	TH1D* th_tempi = new TH1D("Tempi", "Tempi", 300, -100, 200);

	th_tempi -> GetXaxis() -> SetLimits(0, 100);
	th_tempi -> GetXaxis() -> SetRangeUser(0, 100);

	th_tempi -> SetFillColor(kCyan);

	for(int i = 0; i < v_eventi.size(); i++){
		double tdc = v_eventi[i] -> Get_tdc();
		double adc0 = v_eventi[i] -> Get_adc0();
		double adc1 = v_eventi[i] -> Get_adc1();		
		bool adc0_b = adc0 <= maxs1_graph && adc0 >= mins1_graph;
		bool adc1_b = adc1 <= maxs2_graph && adc1 >= mins2_graph;
		if(adc0_b && adc1_b) th_tempi -> Fill(tdc * CONVERSIONETDC);
	}

	c_tempi->cd(1);	

	c_tempi->Modified();	
	c_tempi->Update();	

	th_tempi -> Draw();

	//correzione
	TH1D* th_tempi_corr = new TH1D("Tempi corretti", "Tempi corretti", 300,-100, 200);

	th_tempi_corr -> GetXaxis() -> SetLimits(-100, 200);
	th_tempi_corr -> GetXaxis() -> SetRangeUser(-100, 200);

	th_tempi_corr -> SetFillColor(kCyan);

	for(int i = 0; i < v_eventi.size(); i++){
		double tdc = v_eventi[i] -> Get_tdc_corretto(tau0,vth0,tau1,vth1);
		double adc0 = v_eventi[i] -> Get_adc0();
		double adc1 = v_eventi[i] -> Get_adc1();		
		bool adc0_b = adc0 <= maxs1_graph && adc0 >= mins1_graph;
		bool adc1_b = adc1 <= maxs2_graph && adc1 >= mins2_graph;
		if(adc0_b && adc1_b) th_tempi_corr -> Fill(tdc * CONVERSIONETDC);
	//	cout << tdc * CONVERSIONETDC << endl;
	}

	c_tempi -> cd(2);	
	c_tempi -> Modified();	
	c_tempi -> Update();	
	th_tempi_corr -> Draw();

	if (DEBUG == true)	cout << "\nSTIMA DI V" << endl;
	//----------------------------------------------------------------------
	// 						   stima velocità di m
	//----------------------------------------------------------------------

	TF1 * mygaus = new TF1("mygaus","gaus",0,100);	
	th_tempi_corr -> Fit("mygaus", "L M E");
	double tmedio = mygaus -> GetParameter(1) - RITARDO;
	double sigmat = sqrt(pow(mygaus -> GetParameter(2),2)+pow(E_RIT,2)) / 1000000000;
	tmedio = tmedio / 1000000000;
	double c = dist / tmedio;
	double errc = sqrt(pow(E_DIST/tmedio,2)+pow(c,2)*pow(sigmat,2));

	cout << "\n\n-------------------------------------------\n" << endl;
	cout << "stima della velocità dei muoni (m/s): " << c << " +- " << errc << endl;

	cout << "\n\n-------------------------------------------\n" << endl;
																										//GetParameter(2)/sqrt(th_tempi_corr -> GetEntries())
	cout << "stima del tempo di volo dei muoni: " << mygaus -> GetParameter(1) << " +- " << mygaus -> GetParameter(2) <<  " ns " << endl;


	myApp.Run();
	return 0;
}
