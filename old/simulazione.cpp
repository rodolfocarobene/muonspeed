//c++ -o simulazione simulazione.cpp `root-config --cflags --glibs`

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

#define D 100 //n. bin istogramma
#define N 100000	//numero eventi

#define a 30	//dimensioni riveltori
#define b 80

#define h 171.5	//distanza

#define var 7

double rand_range (const double min, const double max) {
	return min + (max - min) * rand() / RAND_MAX;
}

double distribuzione_angolare (const double& xmin, const double& xmax, const double& ymin, const double& ymax, double inclinazione) {
	double x = 0., y = 0.;
	do {
		x = rand_range(xmin, xmax);
		y = rand_range(ymin, ymax);
	} while (y > pow(cos(x - inclinazione),2));
	return x;
}

double distribuzione_gaussiana (const double& xmin, const double& xmax, const double& ymin, const double& ymax, const double& mean) {
	double x = 0., y = 0.;
	do {
		x = rand_range(xmin, xmax);
		y = rand_range(ymin, ymax);
	} while (y > exp(-pow(x-mean, 2)/(2*var)));
	return x;
}

/*
int main(){
	gStyle->SetOptStat(1111);
	gStyle->SetOptFit(0000);

	double thetamax = atan(sqrt(pow(a, 2) + pow(b, 2))/h);
	double theta, x, y, phi;
	double j = 0;
	double i = 0;
	TApplication* myApp = new TApplication("myApp", NULL, NULL);
	TCanvas* myCanv = new TCanvas("myCanv", "myCanv", 0, 0, 700, 500);
			TH1F* histo = new TH1F("h1", "Distribuzione angolare", D, 0, thetamax);
			histo -> GetXaxis() -> SetTitle("#theta");
			histo -> GetYaxis() -> SetTitle("Eventi");
			TCanvas* myCanv1 = new TCanvas("myCanv1", "myCanv1", 0, 0, 700, 500);
			TH1F* histo1 = new TH1F("h2", "Distribuzione della distanza percorsa", D, 100, h*tan(thetamax));

	TCanvas* c1 = new TCanvas ("c1","c1",0,0,900,600);
	TH2F* histo2 = new TH2F ("histo2","Area efficace (S1)", b/2, 0, b, a, 0, a);
	TCanvas* c2 = new TCanvas ("c2","c2",0,0,900,600);
	TH2F* histo3 = new TH2F ("histo3","Area efficace (S2)", b/2, 0, b, a, 0, a);

	while (i < N) {
		double theta = distribuzione_angolare(0, thetamax, 0, 1, 0);
		histo -> Fill(theta);
		x = rand_range(0, a);	//genero la coordinata e l'angolo di incidenza sul primo rivelatore
		y = rand_range(0, b);
		j = j + 1;
		phi = rand_range(0, 2*M_PI);
		if (x + h*tan(theta)*cos(phi) <= a && x + h*tan(theta)*cos(phi) >= 0 && y + h*tan(theta)*sin(phi) <= b && y + h*tan(theta)*sin(phi) >= 0) { //controllo se interseca il secondo rivelatore
				histo1->Fill(h/cos(theta));
				histo2 -> Fill(y, x);
				histo3 -> Fill(y + h*tan(theta)*sin(phi), x + h*tan(theta)*cos(phi));
				i = i + 1;	//mando avanti il ciclo così che la simulazione si fermi quando l'istogramma ha tanti conteggi quanti quello sperimentale
		}
		else continue;	
	}
	cout << "Rapporto coincidenze/eventi: " << i/j << endl;
	cout << "eventi: " << j << endl;
	
	myCanv -> cd();
	histo -> Draw();
	myCanv1 -> cd();
	histo1 -> Draw();
	c1 -> cd();
	gStyle->SetOptStat(0);
	histo2 -> Draw("COLZ");
	c1->Modified();
 	c1->Update();
	c2 -> cd();
	histo3 -> Draw("COLZ");
	c2->Modified();
 	c2->Update();
	
	myApp->Run();

	return 0;
}*/

int main (int argc, char** argv) {

	/*gStyle->SetOptStat(1111);
	gStyle->SetOptFit(0000);

	double thetamax = atan(sqrt(pow(a, 2) + pow(b, 2))/h);
	double theta, x, y, phi;
	double j = 0;
	double i = 0;
	TApplication* myApp = new TApplication("myApp", NULL, NULL);
	TCanvas* myCanv = new TCanvas("myCanv", "myCanv", 0, 0, 700, 500);
	TH1F* histo = new TH1F("h1", "Distribuzione angolare", D, 0, thetamax);
	histo -> GetXaxis() -> SetTitle("#theta");
	histo -> GetYaxis() -> SetTitle("Eventi");
	TCanvas* myCanv1 = new TCanvas("myCanv1", "myCanv1", 0, 0, 700, 500);
	TH1F* histo1 = new TH1F("h2", "Distribuzione della distanza percorsa", D, 100, h*tan(thetamax));

	TCanvas* c1 = new TCanvas ("c1","c1",0,0,900,600);
	TH2F* histo2 = new TH2F ("histo2","Area efficace (S1)", b/2, 0, b, a, 0, a);
	TCanvas* c2 = new TCanvas ("c2","c2",0,0,900,600);
	TH2F* histo3 = new TH2F ("histo3","Area efficace (S2)", b/2, 0, b, a, 0, a);

	while (i < N) {
		double theta = distribuzione_angolare(0, thetamax, 0, 1);
		histo -> Fill(theta);
		x = rand_range(0, a);	//genero la coordinata e l'angolo di incidenza sul primo rivelatore
		y = rand_range(0, b);
		j = j + 1;
		phi = rand_range(0, 2*M_PI);
		if (x + h*tan(theta)*cos(phi) <= a && x + h*tan(theta)*cos(phi) >= 0 && y + h*tan(theta)*sin(phi) <= b && y + h*tan(theta)*sin(phi) >= 0) { //controllo se interseca il secondo rivelatore
				histo1->Fill(h/cos(theta));
				histo2 -> Fill(y, x);
				histo3 -> Fill(y + h*tan(theta)*sin(phi), x + h*tan(theta)*cos(phi));
				i = i + 1;	//mando avanti il ciclo così che la simulazione si fermi quando l'istogramma ha tanti conteggi quanti quello sperimentale
		}
		else continue;	
	}
	cout << "Rapporto coincidenze/eventi: " << i/j << endl;
	cout << "eventi: " << j << endl;
	
	myCanv -> cd();
	histo -> Draw();
	myCanv1 -> cd();
	histo1 -> Draw();
	c1 -> cd();
	gStyle->SetOptStat(0);
	histo2 -> Draw("COLZ");
	c1->Modified();
 	c1->Update();
	c2 -> cd();
	histo3 -> Draw("COLZ");
	c2->Modified();
 	c2->Update();
	
	myApp->Run();

	return 0;
}*/

// con la correzione dell'efficienza
	gStyle->SetOptStat(1111);
	gStyle->SetOptFit(0000);

	double thetamax = atan(sqrt(pow(a, 2) + pow(b, 2))/h);
	double theta, x, y, phi;
	double j = 0;	//muoni passati
	double i = 0;	//coincidenze viste da S2 (=misurate)
	double k = 0;	//muoni visti da S1
	double l = 0;	//coincidenze
	TApplication* myApp = new TApplication("myApp", NULL, NULL);
	TCanvas* myCanv = new TCanvas("myCanv", "myCanv", 0, 0, 700, 500);
	TH1F* histo = new TH1F("h1", "Distribuzione angolare", D, 0, thetamax);
	histo -> GetXaxis() -> SetTitle("#theta");
	histo -> GetYaxis() -> SetTitle("Eventi");
	TCanvas* myCanv1 = new TCanvas("myCanv1", "myCanv1", 0, 0, 700, 500);
	TH1F* histo1 = new TH1F("h2", "Distribuzione della distanza percorsa", D, 100, h*tan(thetamax));

	TCanvas* c1 = new TCanvas ("c1","c1",0,0,900,600);
	TH2F* histo2 = new TH2F ("histo2","Area efficace (S1)", b/2, 0, b, a, 0, a);
	TCanvas* c2 = new TCanvas ("c2","c2",0,0,900,600);
	TH2F* histo3 = new TH2F ("histo3","Area efficace (S2)", b/2, 0, b, a, 0, a);

	while (i < N) {
		j = j + 1;	//passa un muone
		double theta = distribuzione_angolare(0, thetamax, 0, 1,0);
		histo -> Fill(theta);
		if (k < 0.94*j)	{ //S1 lo vede solo se è entro l'efficienza
			k = k + 1;	//S1 vede un muone
			x = rand_range(0, a);	//genero la coordinata e l'angolo di incidenza sul primo rivelatore
			y = rand_range(0, b);
			phi = rand_range(0, 2*M_PI);
			if (x + h*tan(theta)*cos(phi) <= a && x + h*tan(theta)*cos(phi) >= 0 && y + h*tan(theta)*sin(phi) <= b && y + h*tan(theta)*sin(phi) >= 0) { //controllo se interseca il secondo rivelatore
				l = l + 1;	//c'è una coincidenza
				if (i < 0.93*l) {	//S2 vede la coincidenza solo se è entro l'efficienze
					histo1->Fill(h/cos(theta));
					histo2 -> Fill(y, x);
					histo3 -> Fill(y + h*tan(theta)*sin(phi), x + h*tan(theta)*cos(phi));
					i = i + 1;	//ho visto la coincidenza
				}
				else continue;	
			}
			else continue;
		}
		else continue;
	}
	cout << "Rapporto coincidenze/eventi: " << i/j << endl;
	cout << "efficienza S1 = " << k/j << endl;
	cout << "efficienza S2 = " << i/l << endl;
	
	myCanv -> cd();
	histo -> Draw();
	myCanv1 -> cd();
	histo1 -> Draw();
	c1 -> cd();
	//gStyle->SetOptStat(0);
	histo2 -> Draw("COLZ");
	c1->Modified();
 	c1->Update();
	c2 -> cd();
	histo3 -> Draw("COLZ");
	c2->Modified();
 	c2->Update();
	
	myApp->Run();

	return 0;

}
/*
	
//correzione efficienza e gaussiana
	gStyle->SetOptStat(1111);
	gStyle->SetOptFit(0000);

	double thetamax = atan(sqrt(pow(a, 2) + pow(b, 2))/h);
	double theta, x, y, phi;
	double j = 0;	//muoni passati
	double i = 0;	//coincidenze viste da S2 (=misurate)
	double k = 0;	//muoni visti da S1
	double l = 0;	//coincidenze
	TApplication* myApp = new TApplication("myApp", NULL, NULL);
	TCanvas* myCanv = new TCanvas("myCanv", "myCanv", 0, 0, 700, 500);
	TH1F* histo = new TH1F("h1", "Distribuzione angolare", D, 0, thetamax);
	histo -> GetXaxis() -> SetTitle("#theta");
	histo -> GetYaxis() -> SetTitle("Eventi");
	TCanvas* myCanv1 = new TCanvas("myCanv1", "myCanv1", 0, 0, 700, 500);
	TH1F* histo1 = new TH1F("h2", "Distribuzione della distanza percorsa", D, 50, h/cos(thetamax));
	TCanvas* myCanv1bis = new TCanvas("myCanv1bis", "myCanv1bis", 0, 0, 700, 500);
	TH1F* histo1bis = new TH1F("h2bis", "Distribuzione della distanza percorsa con correzione gaussiana", D, 60, 118);


	TCanvas* c1 = new TCanvas ("c1","c1",0,0,900,600);
	TH2F* histo2 = new TH2F ("histo2","Area efficace (S1)", b/2, 0, b, a, 0, a);
	TCanvas* c2 = new TCanvas ("c2","c2",0,0,900,600);
	TH2F* histo3 = new TH2F ("histo3","Area efficace (S2)", b/2, 0, b, a, 0, a);

	double max = 0;
	double min = 10000;
	double d;
	while (i < N) {
		j = j + 1;	//passa un muone
		double theta = distribuzione_angolare(0, thetamax, 0, 1, 0);
		histo -> Fill(theta);
		if (k < 0.94*j)	{ //S1 lo vede solo se è entro l'efficienza
			k = k + 1;	//S1 vede un muone
			x = rand_range(0, a);	//genero la coordinata e l'angolo di incidenza sul primo rivelatore
			y = rand_range(0, b);
			phi = rand_range(0, 2*M_PI);
			if (x + h*tan(theta)*cos(phi) <= a && x + h*tan(theta)*cos(phi) >= 0 && y + h*tan(theta)*sin(phi) <= b && y + h*tan(theta)*sin(phi) >= 0) { //controllo se interseca il secondo rivelatore
				l = l + 1;	//c'è una coincidenza
				if (i < 0.9344*l) {	//S2 vede la coincidenza solo se è entro l'efficienze
					histo1->Fill(h/cos(theta));
					d = (h/cos(theta)) + gRandom -> Gaus(0, var);
					histo1bis -> Fill(d);
					if (d > max) max = d;
					if (d < min) min = d;
					histo2 -> Fill(y, x);
					histo3 -> Fill(y + h*tan(theta)*sin(phi), x + h*tan(theta)*cos(phi));
					i = i + 1;	//ho visto la coincidenza
				}
				else continue;	
			}
			else continue;
		}
		else continue;
	}

	TCanvas* myCanv1biss = new TCanvas("myCanv1biss", "myCanv1biss", 0, 0, 700, 500);
	TH1F* histo1biss = new TH1F("h2biss", "Distribuzione della distanza percorsa con correzione gaussiana", D, 100, max);
	for(unsigned int i = 1; i < N; i++)	histo1biss -> Fill(i, histo1bis->GetBinContent(i));
	myCanv1biss -> cd();
	histo1biss -> Draw();

	cout << "minimo: " << min << endl;
	cout << "massimo: " << max << endl;
	cout << "coincidenze: " << i << endl;
	cout << "eventi: " << j << endl;
	cout << "Rapporto coincidenze/eventi: " << i/j << endl;
	cout << "efficienza S1 = " << k/j << endl;
	cout << "efficienza S2 = " << i/l << endl;
	
	myCanv -> cd();
	histo -> Draw();
	myCanv1 -> cd();
	histo1 -> Draw();
	myCanv1bis -> cd();
	histo1bis -> Draw();

	c1 -> cd();
	//gStyle->SetOptStat(0);
	histo2 -> Draw("COLZ");
	c1->Modified();
 	c1->Update();
	c2 -> cd();
	histo3 -> Draw("COLZ");
	c2->Modified();
 	c2->Update();

	TGraphErrors* grafico = new TGraphErrors(argv[1]);
	TCanvas* myC = new TCanvas("myC","myC",0,0,700,500);
	myC -> cd();
	grafico -> Draw("AP");
	
	myApp->Run();

	return 0;

}
	*/
//correzione efficienza e gaussiana flusso 
/*
	gStyle->SetOptStat(0000);
	gStyle->SetOptFit(0000);


	double theta, x, y, phi;
	double j = 0;	//muoni passati in 16m2
	double p = 0;	//muoni passati da S1
	double i = 0;	//coincidenze viste da S2 (=misurate)
	double k = 0;	//muoni visti da S1
	double l = 0;	//coincidenze
	double m = 0;   //muoni passati da S2
	TApplication* myApp = new TApplication("myApp", NULL, NULL);
	TCanvas* myCanv = new TCanvas("myCanv", "myCanv", 0, 0, 700, 500);
	TH1F* histo = new TH1F("h1", "Distribuzione angolare", D, 0, M_PI/2);
	histo -> GetXaxis() -> SetTitle("#theta");
	histo -> GetYaxis() -> SetTitle("Eventi");
	TCanvas* myCanv1 = new TCanvas("myCanv1", "myCanv1", 0, 0, 700, 500);
	TH1F* histo1 = new TH1F("h2", "Distribuzione della distanza percorsa", D, 50, h/cos(M_PI/2));
	TCanvas* myCanv1bis = new TCanvas("myCanv1bis", "myCanv1bis", 0, 0, 700, 500);
	TH1F* histo1bis = new TH1F("h2bis", "Distribuzione della distanza percorsa con correzione gaussiana", D, 50, h/cos(M_PI/2));

	//coincidenze in funzione della distanza fra i rivelatori
	TGraphErrors* grafico = new TGraphErrors();
	TCanvas* myC = new TCanvas("myC","myC",0,0,700,500);
	
	int point = 0;
	double inclinazione;
	TCanvas* c1 = new TCanvas ("c1","c1",0,0,900,600);
	TH2F* histo2 = new TH2F ("histo2","Area efficace (S1)", b/2, 0, b, a, 0, a);
	TCanvas* c2 = new TCanvas ("c2","c2",0,0,900,600);
	TH2F* histo3 = new TH2F ("histo3","Area efficace (S2)", b/2, 0, b, a, 0, a);
	for (inclinazione = -M_PI/2; inclinazione <= M_PI/2; inclinazione = inclinazione + M_PI/36) {
		j = 0;	//muoni passati in 16m2
		p = 0;	//muoni passati da S1
		i = 0;	//coincidenze viste da S2 (=misurate)
		k = 0;	//muoni visti da S1
		l = 0;	//coincidenze
		m = 0;  //muoni passati per S2
		double thetamax = atan(sqrt(pow(a, 2) + pow(b, 2))/h);
			while (j < N) {
				j = j + 1;	//passa un muone
				double theta = distribuzione_angolare(- M_PI/2 + inclinazione, M_PI/2 + inclinazione, 0, 1, inclinazione);
				histo -> Fill(theta);	
				x = rand_range(0, a);	//genero la coordinata e l'angolo di incidenza sul primo rivelatore
				y = rand_range(0, b);
				phi = rand_range(0, 2*M_PI);
				if (x + h*tan(theta)*cos(phi) <= a && x + h*tan(theta)*cos(phi) >= 0 && y + h*tan(theta)*sin(phi) <= b && y + h*tan(theta)*sin(phi) >= 0) {	//un muone è passato per S2
					k += 1; //evento visto da S1
					if (x + h*tan(theta)*cos(phi) <= a && x + h*tan(theta)*cos(phi) >= 0 && y + h*tan(theta)*sin(phi) <= b && y + h*tan(theta)*sin(phi) >= 0) { //controllo se interseca il secondo rivelatore
						l = l + 1;	//c'è una coincidenza
						if (i < 0.9344*l) {	//S2 vede la coincidenza solo se è entro l'efficienza
							histo1->Fill(h/cos(theta));
							histo1bis -> Fill((h/cos(theta))+gRandom -> Gaus(0, var));
							histo2 -> Fill(y, x);
							histo3 -> Fill(y + h*tan(theta)*sin(phi), x + h*tan(theta)*cos(phi));
							i = i + 1;	//ho visto la coincidenza
						}
					}
				}
			}
			cout <<  "inclinazione: " << inclinazione << ", coincidenze: " << i << endl;	
			grafico -> SetPoint(point, inclinazione, i/280030);
			point += 1;
	}

	cout << "coincidenze: " << i << endl;
	cout << "eventi visti da S2:" << m << endl;


	myCanv -> cd();
	histo -> Draw();
	myCanv1 -> cd();
	histo1 -> Draw();
	myCanv1bis -> cd();
	histo1bis -> Draw();

	c1 -> cd();
	//gStyle->SetOptStat(0);
	histo2 -> Draw("COLZ");
	c1->Modified();
 	c1->Update();
	c2 -> cd();
	histo3 -> Draw("COLZ");
	c2->Modified();
 	c2->Update();

	grafico -> SetMarkerStyle(20);
	myC -> cd();
	grafico -> Draw("AP");
	
	myApp->Run();

	return 0;

}*/