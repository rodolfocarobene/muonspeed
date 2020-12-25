//c++ -o sigmoide_o_retta sigmoide_o_retta.cpp `root-config --cflags --glibs`

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

//-------------definizioni uniche per il fit lineare
#define MIN -140			//minima x per il fit
#define MAX +146			//massima "    "
#define SALTO_I -69			//da questa x inizia il salto
#define SALTO_F +75			//"         " finisce "  "
//---------------------------

#define SIGMOID 0 		//se è 1 fitta la sigmoide, 
				  		//se è zero (o altro) fa il fit lineare

double myFun(double *x, double *par){		//sigmoide
	double A = par[0];
	double B = par[1];
	double C = par[2];
	double num = C;
	double den = 1 + exp((A-abs(x[0]))/B);
	return num/den;
}

double myLine(double *x, double *par){		//retta con salto
	double A = par[0];
	if(x[0] > SALTO_I && x[0] < SALTO_F){
		TF1::RejectPoint();
		return 0;
	}
	return A;
}


int main (int argc, char** argv)
{
	TApplication myApp("myApp",NULL,NULL);
	gStyle -> SetOptFit(0111);

	TGraph* gr= new TGraph("data.txt","%lg %lg %*lg");
	gr->SetTitle("Nome grafico");
	gr->SetMarkerColor(4);//blu
	gr->SetMarkerSize(1);
	gr->SetMarkerStyle(8);

//Non funziona --- ma basta dividerei dati a mano /excel
/* Sad Face
	___		___
੨	---		---
	|||	 |  |||
		---

*/
//	TF1 *normalize = new TF1("normalize","x/180", -500,+500);
//	gr -> Apply(normalize);
	
	TCanvas* c1 = new TCanvas("c1","c1",0,0,700,500);
	c1->cd();

	gr->Draw("AP");

	if(SIGMOID == 1){
		TF1 *sigmoid = new TF1("sigmoid",myFun, -200,+200,3);
		sigmoid -> SetParameter(0, 1);
		sigmoid -> SetParameter(1, -50);
		sigmoid -> SetParameter(2, 10000);
		gr -> Fit("sigmoid");
	}
	else{
		TF1 *line_neg = new TF1("line_neg","pol0", MIN,SALTO_I);
		TF1 *line_pos = new TF1("line_pos","pol0", SALTO_F,MAX);
		TF1 *line = new TF1("line",myLine,MIN,MAX, 1);

		gr -> Fit("line", "NR");

		double rate = line -> GetParameter(0);
		line_pos -> FixParameter(0,rate);
		line_neg -> FixParameter(0,rate);

		gr -> Fit("line_pos", "R");
		gr -> Fit("line_neg", "R+");
	}

	c1->Modified();
	c1->Update();
		

	myApp.Run();
	return 0;
}


/* scrivo qua i dati che ho usato così non si perdono

##data.txt

-207	0
-155	6
-103	24
-81	18
-71	17
-61	15
-51	32
-30	763
-20	1883
-10	4023
0	4183
10	4199
20	2468
30	1141
51	104
61	77
71	77
81	23
103	15
155	4
207	0

*/