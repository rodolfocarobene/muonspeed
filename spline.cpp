//c++ -o spline spline.cpp `root-config --cflags --glibs`

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

#define strtk_no_tr1_or_boost 1
#include "strtk.hpp"

#define STEP 0.1

using namespace std;

vector<double> traccia0;
vector<double> traccia1;

bool read_website(char* myFile){

	vector<string> lettura;

	string line;
	ifstream myfile (myFile);
	string token;
	int pos;
	if(myfile.is_open()){
		getline(myfile, line);
		strtk::parse(line, "," ,lettura);
		myfile.close();
	}
	else return false;

	if(traccia0.size() == 0){
		for(int i = 0; i < lettura.size();i++){
			traccia0.push_back(-stod(lettura[i]));
		}
	}
	else{
		for(int i = 0; i < lettura.size();i++){
			traccia1.push_back(-stod(lettura[i]));
		}
	}
	

	return true;
}

double myFun(double *x, double *par){		
	double A = par[0];		//amp
	double B = par[1];		//tau
	double C = par[2];		//offset
	double w = x[0];
	return A*(1-exp((w-C)/B));
}

double mySigm(double *x, double *par){			
	double A = par[0];		//amp
	double B = par[1];		//tau
	double C = par[2];		//offset
	double w = x[0];
	return A/(1+exp(-B*(w-C)));
}

double newFun(double *x, double *par){			
	double A = par[0];		//amp
	double B = par[1];		//tau
	double C = par[2];		//offset
	double w = x[0];
	return A*(1/exp(-B*(w-C)));
}


int main(int argc, char *argv[]){

	string path0 = argv[1];

	read_website(&path0[0]);

	cout << "Punti in traccia: " << traccia0.size() << endl;

	TApplication myApp("myApp",NULL,NULL);
	gStyle -> SetOptFit(1111);
	gStyle -> SetOptStat(1111);



	TCanvas* c_1 = new TCanvas("c_1","c_1",0,0,700,500);
	TGraphErrors * g_1 = new TGraphErrors;

	double x = 0;

	for(int i = 0; i < traccia0.size(); i++){
		double y = traccia0[i];
		int N = g_1 -> GetN();			
		g_1 -> SetPoint(N,x,y);
		g_1 -> SetPointError(N,x/100,y/100);
		x = x + STEP;
	}
	g_1 -> Draw();



	TF1 * fun_std_0 = new TF1("fun_adc0",myFun, 10,20,3);
	TF1 * fun_sigm_0 = new TF1("fun_sigm_0",mySigm, 10,20,3);
	TF1 * fun_new = new TF1("fun_new",newFun, 10,20,3);



/*
	fun_std_0 -> SetLineColor(kRed);

	fun_std_0 -> SetParName(0, "Amp0");
	fun_std_0 -> SetParName(1, "Tau0");
	fun_std_0 -> SetParName(2, "Off0");

	fun_std_0 -> SetParameter(0, -3);
	fun_std_0 -> SetParameter(1, 3);
	fun_std_0 -> SetParameter(2, 3);

	TVirtualFitter::SetMaxIterations(20000);

	TFitResultPtr r_0 = g_1 -> Fit("fun_std_0", "R S");

*/


	myApp.Run();
	return 0;
}