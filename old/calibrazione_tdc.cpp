//c++ -o calibrazione_tdc calibrazione_tdc.cpp `root-config --cflags --glibs`

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

using namespace std;

#define MODE 0 	//0 fitta con un polinomio
				//1 fitta con la retta nell'intervallo
#define MIN 10
#define MAX 20

int main()
{
	TApplication myApp("myApp",NULL,NULL);
	gStyle -> SetOptFit(1111);

	TGraph* gr= new TGraph("data_tdc.txt","%lg %lg %*lg");
	gr->SetTitle("Nome grafico");
	gr->SetMarkerColor(4);//blu
	gr->SetMarkerSize(1);
	gr->SetMarkerStyle(8);
	TCanvas* c1 = new TCanvas("c1","c1",0,0,700,500);
	c1->cd();
	gr->Draw("AP");

	if(MODE == 0){
		TF1 *polin = new TF1("polin","pol1",0,100);		//scrivere il polinomio
		gr -> Fit("polin");
	}
	if(MODE == 1){
		TF1 *retta = new TF1("retta", "pol0", MIN, MAX);
		gr -> Fit("retta", "R");
	}
	c1->Modified();
	c1->Update();
	myApp.Run();
	return 0;
}