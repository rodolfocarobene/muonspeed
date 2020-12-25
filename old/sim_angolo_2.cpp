//c++ -o sim_angolo_2 sim_angolo_2.cpp `root-config --cflags --glibs`

//Questa differisce dalla precedente perché metto il rivelatore centrato in 0,0,0, e lo faccio ruotare attorno all'asse x

#include <iostream>
#include <ctime>
#include <cmath>
#include <fstream>
#include <vector>

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TF1.h"
#include "TMath.h"
#include "TLine.h"
#include "TStyle.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TAxis.h"
#include "TLatex.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TLegend.h"
using namespace std;

#define N_muoni 10000
#define N_max 100000000*5
#define a 0.8	//dimensione x riveltori
#define b 0.3 //dimensione y rivelatore

#define debug true

//Funzione che genera valore casuale secondo distribuzione uniforme
double rand_range (const double min, const double max) {
	return min + (max - min) * rand() / RAND_MAX;
}

//Funzione che genera valore casuale secondo distribuzione cos^2(theta)		Try and catch 		Theta è compreso fra pi/2 e 0  per ragioni legate al sistema di coordinate
double distribuzione_angolare () {
	double x = 0., y = 0.;
	do {
		x = rand_range(0, M_PI/2);
		y = rand_range(0, 1);
	} while (y > pow(cos(x),2));
	return x;
}

//Funzione per eseguire il fit psi contro theta
double cos_2 (double* x, double* par){
	return par[0]+par[1]*pow(cos(x[0]),2);
}

//Funzione per simulare efficienza
bool efficienza (const double efficienza) {
	if(rand_range(0,1)< efficienza)
		return true;
	else return false;
}

//Intersenzione piano 1 + verifica che sia interno rettangolo rivelatore
bool piano_1 (double x_1,double y_1,double X_max,double Y_max,double theta,double phi,double psi, double* x, double* y){
	//Intersezione
	double r=(b/2-y_1*tan(psi))/(cos(theta)+tan(psi)*sin(theta)*sin(phi));
    x[0]=r*sin(theta)*cos(phi)+x_1;
    y[0]=r*sin(theta)*sin(phi)+y_1;
    double z=b/2-r*cos(theta);

 	//Interno rettangolo
 	if (x[0]<=a/2 && x[0]>=-a/2&& y[0]<=b/2*cos(psi) && y[0]>=-b/2*cos(psi)){
 		y[0]=y[0]/cos(psi);
      	return true;
    }
  	else
      	return false;
}

//intersezione piano 2 + verifica che sia interno rettangolo rivelatore
bool piano_2 (double x_1,double y_1,double X_max,double Y_max,double theta,double phi,double psi,double L,double* x, double* y){
	
	double r=(b/2-y_1*tan(psi)+L/cos(psi))/(cos(theta)+tan(psi)*sin(theta)*sin(phi));
    x[0]=r*sin(theta)*cos(phi)+x_1;
    y[0]=r*sin(theta)*sin(phi)+y_1;
    double z=b/2-r*cos(theta);


	if (x[0]<=a/2 && x[0]>=-a/2&& y[0]<=b/2*cos(psi)+L*sin(psi) && y[0]>=-b/2*cos(psi)+L*sin(psi)){
        y[0]=(y[0]-L*sin(psi)+b/2*cos(psi))/cos(psi)-b/2;
      	return true;
    }
  	else
     	return false;
	
}



int main (int argc, char** argv) {

	//
	//
	//   OPZIONI GRAFICI
	//
	//
	gStyle->SetOptFit(1111);
	gStyle->SetOptStat(0);
	gStyle->SetPalette(1);

	TApplication* myApp = new TApplication("myApp", NULL, NULL);
	TCanvas* myCanv = new TCanvas("myCanv", "myCanv", 0, 0, 700, 500);

	TGraphErrors* graf_psi_theta = new TGraphErrors();
	graf_psi_theta -> GetXaxis() -> SetTitle("#psi");
	graf_psi_theta -> GetYaxis() -> SetTitle("#theta");
	int point=0; //Serve poi il tgraph


	TCanvas* c1 = new TCanvas ("c1","c1",0,0,900,600);
	//TH2F* histo2 = new TH2F ("histo2","Area efficace (S1)", 32, -b/2, b/2, 12, -a/2, a/2);
	TCanvas* c2 = new TCanvas ("c2","c2",0,0,900,600);
	//TH2F* histo3 = new TH2F ("histo3","Area efficace (S2)", 32, -b/2, b/2, 12, -a/2, a/2);

	//
	//
	//   SIMULAZIONE VERA E PROPRIA
	//
	//


	// theta, phi, psi angolo zenitale, angolo azimutale, angolo fra i rivelatori
	
	double eff_S1_mean=0.94;
	double erro_S1=0.0011;
	double eff_S2_mean=0.9344;
	double erro_S2=0.0012;

	double X_max=0.3; double Y_max=0.8; //dimensione rivelatore anche questa è da ripensare 

	double mean_L=0.30; double sig_L=0.00; //distanza rivelatore

	int ang=-80; //mi serve nella stampa dopo
	for( double psi=-M_PI/2+M_PI/18; psi<=M_PI/2; psi=psi+M_PI/18){ // questa è l'inclinazione psi=-M_PI/2+M_PI/18

		TH2F* histo2 = new TH2F ("histo2","Area efficace (S1)", 32, -a/2, a/2, 12, -b/2, b/2);
		TH2F* histo3 = new TH2F ("histo3","Area efficace (S2)", 32, -a/2, a/2, 12, -b/2, b/2);
		
		int countS1=0;
	    int countS2=0;
	    int coincidenze_doppie=0;
	    int n_muon=0;
	    int count=0;
		while (countS1 < 500000) {

			bool S_1=false; //diventa true se muone passa per i rivelatore
			bool S_2= false;

			//Genero il punto, che sarà dato da X_1, Y_1, theta e psi

			double X_1 = rand_range(-3,3);	//genero le coordinate di impatto (z=b/2)   FUNZIONANO GIUSTE
			double Y_1 = rand_range(-8, 8);
			double phi = rand_range(0, 2*M_PI); //angolo azimutale
			
			double theta = distribuzione_angolare(); //genera theta fra 0 e pi/2 con distribuzione il coseno quadro

			//Verifico che transitino per il rivelatore 1
			double x_1[1];
			double y_1[1];
			 //punti di impatto sul rivelatore 1
			if (piano_1 (X_1, Y_1, X_max, Y_max, theta, phi, psi, x_1, y_1)==true){
				n_muon++;
				if (efficienza(eff_S1_mean)==true)
				{
					countS1++;
					S_1=true;
				}
			}

			double x_2[1];
			double y_2[1]; //punti di impatto sul rivelatore 2
			if(piano_2(X_1, Y_1, X_max, Y_max, theta, phi, psi, mean_L, x_2, y_2)==true && efficienza(eff_S2_mean)==true){
				countS2++;
				S_2= true;
			}

			if (S_1==true && S_2==true){
				coincidenze_doppie++;
				histo2 -> Fill(x_1[0],y_1[0]);
																		//if(debug==true) cout<< "X_1: "<<x_1[0]<<" Y_1: "<<y_1[0]<<endl;
				histo3 -> Fill(x_2[0],y_2[0]);     
																		//if(debug==true) cout<< "X_2: "<<x_2[0]<<" Y_2: "<<y_2[0]<<endl;
			}
			if (count>=N_max) break;
			count++;
		}

																		if(debug==true) cout<< "psi: "<<psi*180/M_PI<<" countS1: "<<countS1<<" count: "<<count<<" coincidenze_doppie: "<<coincidenze_doppie<<endl;
		graf_psi_theta->SetPoint(point, psi, coincidenze_doppie);
		graf_psi_theta->SetPointError(point, 0, sqrt(coincidenze_doppie));
		point++;

		//
		//
		//Print delle aeree efficaci
		//
		//
		c1 -> cd();
		histo2 -> Draw("COLZ");
		c1->Modified();
	 	c1->Update();
		c2 -> cd();
		histo3 -> Draw("COLZ");
		c2->Modified();
	 	c2->Update();

		string nome_file_1 ="Riv1-.png";
		string nome_file_2 ="Riv2-.png";

		string angol= to_string (ang);
		nome_file_1.insert(5,angol);
		nome_file_2.insert(5,angol);
		

		const char *riv1 = nome_file_1.c_str();
		const char *riv2 = nome_file_2.c_str();

		c1->Print(riv1,"png");
		c2->Print(riv2,"png");
		ang= ang+10;

		histo2 -> ~TH2F();
		histo3 -> ~TH2F();


	}
	

	//
	//
	//	STAMPA
	//
	//
	
	myCanv -> cd();
	graf_psi_theta -> SetMarkerStyle(20);
	graf_psi_theta -> Draw("AP");

	//
	// FIT con cos^2
	//
	TF1* myFun = new TF1("myFun", cos_2, -M_PI/2, 0, 2); 
	myFun->SetParameters(10000,500000);
	myFun->SetParNames("offset", "A");
	graf_psi_theta -> Fit("myFun");

	
	myApp->Run();

	return 0;
}

