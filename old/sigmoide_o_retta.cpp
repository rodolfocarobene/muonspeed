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
#define MIN -156			//minima x per il fit
#define MAX +105			//massima "    "
#define SALTO_I -60			//da questa x inizia il salto
#define SALTO_F +82			//"         " finisce "  "
//---------------------------

#define SIGMOID 1 		//se è 1 fitta la sigmoide, 
				  		//se è zero (o altro) fa il fit lineare


double myFun(double *x, double *par){		//sigmoide
	double A = par[0];
	double B = par[1];
	double C = par[2];
	double D = par[3];
	double num = C;
	double den = 1 + exp((A-abs(x[0] - D))/B);
	return num/den;
}

TF1 * media_altezza = new TF1("media_altezza", "pol0", 0, 200);
TF1 * sigmoid = new TF1("sigmoid",myFun, -200,+200,4);

double myLine(double *x, double *par){		//retta con salto
	double A = par[0];
	if(x[0] > SALTO_I && x[0] < SALTO_F){
		TF1::RejectPoint();
		return 0;
	}
	return A;
}

double finter(double *x, double *par) 
{
    return TMath::Abs(sigmoid->EvalPar(x,par) - media_altezza->EvalPar(x,par));
}

double fint(double start,double end) {                                
       TF1 *fint = new TF1("fint",finter,start,end,0);
       double xint = fint->GetMinimumX();
       return xint; 
}

int main (int argc, char** argv)
{
	TApplication myApp("myApp",NULL,NULL);
	gStyle -> SetOptFit(1111);

	TGraph* gr= new TGraph("data_accidentali.txt","%lg %lg %*lg");
	gr->SetTitle("Nome grafico");
	gr->SetMarkerColor(4);//blu
	gr->SetMarkerSize(1);
	gr->SetMarkerStyle(8);

	TCanvas* c1 = new TCanvas("c1","c1",0,0,700,500);
	c1->cd();

	gr->Draw("AP");

	if(SIGMOID == 1){
		
		sigmoid -> SetParameter(0, 1);
		sigmoid -> SetParameter(1, -50);
		sigmoid -> SetParameter(2, 10000);
		sigmoid -> SetParameter(3, 3);
		gr -> Fit("sigmoid");

		double max_y= sigmoid -> Eval(sigmoid -> GetParameter(1)) ;
		media_altezza -> SetParameter(0, max_y / 2);
		double a = fint(-100,0);
		double b = fint(0,100);
		cout << "x_int = " << a << endl;
		cout << "x_int = " << b << endl;
		cout << "delta = " << b - a << endl;

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

	c1->SetGrid();
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