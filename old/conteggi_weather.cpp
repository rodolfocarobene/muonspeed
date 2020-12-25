//c++ -o conteggi_weather conteggi_weather.cpp `root-config --cflags --glibs`

// ATTENZIONE, aggiungere al file di dati una riga di 12 zeri

//esempio di riga di input 
/*

S1      S2            doppie   triS1   triS2
8000	1392	0	0	15	0	15	    15	0	0	0	0

*/


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
#include <TH1D.h>

#define START 13 			//ora di start della misura
#define TRAMONTO 17		//ora in cui tramonta il sole
#define ALBA 7.5 				//ora a cui sorge il sole

#define DELTAT_BIN 100		//larghezza dei bin     -------------deve essere un multiplo preciso?
#define MIN_BIN 2000		//minimo x degli istogrammi
#define MAX_BIN 5000		//massimo x degli istogrammi

using namespace std;

bool leggi_dati(vector<double> & conteggiS1, vector<double> & conteggiS2, vector<double> & doppie, vector<double> & tripleS1, vector<double> & tripleS2, int argc, char** argv);

int main(int argc, char *argv[])
{
	TApplication myApp("myApp",NULL,NULL);
	gStyle -> SetOptFit(1111);

	//vettori per le variabili da memorizzare
	vector<double> conteggiS1, conteggiS2, doppie, tripleS1, tripleS2;
	vector<double> ore;

	//lettura del file di dati
	if( !(leggi_dati(conteggiS1, conteggiS2, doppie, tripleS1, tripleS2, argc, argv)) ) return 1;

	for(int i = 0; i < conteggiS1.size(); i++) ore.push_back(i + 1);

	//grafico a dispersione: 
	//						sull'asse delle x il numero della riga del file (l'ora)
	//						sull'asse delle y i conteggi in quell'ora

		//conteggi di S1
		
	TGraph* gr_1= new TGraph;
	for(int i = 1; i < conteggiS1.size(); i++){
		double x = i;
		int N = gr_1 -> GetN();
		double y = (conteggiS1[i] - conteggiS1[i-1])/60;
		gr_1 -> SetPoint(N,x,y);
	}

	gr_1->SetTitle("Conteggi di S1 nel tempo");
	gr_1->SetMarkerColor(4);//blu
	gr_1->SetMarkerSize(1);
	gr_1->SetMarkerStyle(8);

	TCanvas* c_1 = new TCanvas("c_1","c_1",0,0,700,500);
	c_1->cd();

	gr_1->Draw("AP");

	//conteggi di S2
																				//	TVectorD v_conteggiS2(conteggiS2.size(),&conteggiS2[0]);	//prima o poi potrebbe essere utile questa sintassi

	TGraph* gr_2= new TGraph;
	for(int i = 1; i < conteggiS2.size(); i++){
		double x = i;
		int N = gr_2 -> GetN();
		double y = (conteggiS2[i] - conteggiS2[i-1])/60;
		gr_2 -> SetPoint(N,x,y);
	}

	gr_2->SetTitle("Conteggi di S2 nel tempo");
	gr_2->SetMarkerColor(4);//blu
	gr_2->SetMarkerSize(1);
	gr_2->SetMarkerStyle(8);

	TCanvas* c_2 = new TCanvas("c_2","c_2",0,0,700,500);
	c_2->cd();

	gr_2->Draw("AP");

	//----------------------------------------------------------
	//			    	GRAFICI GIORNO - NOTTE
	//----------------------------------------------------------

	//grafico a dispersione per S1 con due colori per giorno e notte
	TGraph* gr_3 = new TGraph;		//grafico del giorno
	TGraph* gr_4 = new TGraph;		//grafico dela notte

	for(int i = 1; i < conteggiS1.size(); i++){
		double x = i;

		int ora = (START + i) % 24;
		bool notte = false;
		if (ora >= TRAMONTO || ora <= ALBA) notte = true;
		if (notte == true){
			int N = gr_4 -> GetN();
			double y = (conteggiS1[i] - conteggiS1[i-1])/60;
			gr_4 -> SetPoint(N,x,y);
		}
		else{
			int N = gr_3 -> GetN();
			double y = (conteggiS1[i] - conteggiS1[i-1])/60;
			gr_3 -> SetPoint(N,x,y);
		}
	}

	//gr_3->SetTitle("Conteggi di S1 nel tempo (giorno giallo vs notte nera)");
	gr_3->SetMarkerColor(5);//giallo
	gr_3->SetMarkerSize(0.5);
	gr_3->SetMarkerStyle(8);	

	gr_4->SetMarkerColor(1);//nero
	gr_4->SetMarkerSize(0.5);
	gr_4->SetMarkerStyle(8);

	TCanvas* c_3 = new TCanvas("c_3","c_3",0,0,700,500);
	c_3->cd();

	gr_3 -> GetYaxis() -> SetRangeUser(3000,4500);
	gr_3->Draw("AP");
	gr_4->Draw("SAME P");

	//grafico a dispersione per S2 con due colori er giorno e notte
	TGraph* gr_5 = new TGraph;		//grafico del giorno
	TGraph* gr_6 = new TGraph;		//grafico dela notte

	for(int i = 1; i < conteggiS2.size(); i++){
		double x = i;

		int ora = (START + i) % 24;
		bool notte = false;
		if (ora >= TRAMONTO || ora <= ALBA) notte = true;
		if (notte == true){
			int N = gr_6 -> GetN();
			double y = (conteggiS2[i] - conteggiS2[i-1])/60;
			gr_6 -> SetPoint(N,x,y);
		}
		else{
			int N = gr_5 -> GetN();
			double y = (conteggiS2[i] - conteggiS2[i-1])/60;
			gr_5 -> SetPoint(N,x,y);
		}
	}

	//gr_5->SetTitle("Conteggi di S2 nel tempo (giorno giallo vs notte nera)");
	gr_5->SetMarkerColor(5);//giallo
	gr_5->SetMarkerSize(0.5);
	gr_5->SetMarkerStyle(8);	

	gr_6->SetMarkerColor(1);//nero
	gr_6->SetMarkerSize(0.5);
	gr_6->SetMarkerStyle(8);

	TCanvas* c_4 = new TCanvas("c_4","c_4",0,0,700,500);
	c_4->cd();

	gr_5 -> GetYaxis() -> SetRangeUser(3000,4500);
	gr_5->Draw("AP");
	gr_6->Draw("SAME P");

	//istogramma giorno-notte per S1
	TH1D* th_1 = new TH1D("Dispersione del rate col sole (S1)", "Dispersione del rate col sole (S1)", (MAX_BIN - MIN_BIN) / DELTAT_BIN, MIN_BIN, MAX_BIN);		//istogramma del giorno
	TH1D* th_2 = new TH1D("Dispersione del rate senza sole (S1)", "Dispersione del rate senza sole (S1)", (MAX_BIN - MIN_BIN) / DELTAT_BIN, MIN_BIN, MAX_BIN);	//istogramma della notte

	bool warning_histoS1 = false;

	for(int i = 1; i < conteggiS1.size(); i++){
		int ora = (START + i) % 24;
		bool notte = false;
		if (ora >= TRAMONTO || ora <= ALBA) notte = true;
		if (notte == true){
			double y = (conteggiS1[i] - conteggiS1[i-1])/60;			//il giorno viene fillato sempre e comunque
			th_2 -> Fill(y);
			th_1 -> Fill(y);
			if (y > MAX_BIN || y < MIN_BIN) warning_histoS1 = true;
		}
		else{
			double y = (conteggiS1[i] - conteggiS1[i-1])/60;
			th_1 -> Fill(y);
			if (y > MAX_BIN || y < MIN_BIN) warning_histoS1 = true;
		}
	}

	th_1->SetFillColor(5);//giallo
	th_2->SetFillColor(1);//nero

	TCanvas* c_5 = new TCanvas("c_5","c_5",0,0,700,500);
	c_5->cd();

	th_1->Draw();
	th_2->Draw("SAME");

	if (warning_histoS1 == true) cout << "WARNING: Almeno una misura esce dai limiti imposti all'istogramma giorno-notte di S1" << endl;
	
	//istogramma giorno-notte per S2
	TH1D* th_3 = new TH1D("Dispersione del rate col sole (S2)", "Dispersione del rate col sole (S2)", (MAX_BIN - MIN_BIN) / DELTAT_BIN, MIN_BIN, MAX_BIN);		//istogramma del giorno
	TH1D* th_4 = new TH1D("Dispersione del rate senza sole (S2)", "Dispersione del rate senza sole (S2)", (MAX_BIN - MIN_BIN) / DELTAT_BIN, MIN_BIN, MAX_BIN);	//istogramma della notte

	bool warning_histoS2 = false;

	for(int i = 1; i < conteggiS1.size(); i++){
		int ora = (START + i) % 24;
		bool notte = false;
		if (ora >= TRAMONTO || ora <= ALBA) notte = true;
		if (notte == true){
			double y = (conteggiS2[i] - conteggiS2[i-1])/60;			//il giorno viene fillato sempre e comunque
			th_4 -> Fill(y);
			th_3 -> Fill(y);
			if (y > MAX_BIN || y < MIN_BIN) warning_histoS2 = true;
		}
		else{
			double y = (conteggiS2[i] - conteggiS2[i-1])/60;
			th_3 -> Fill(y);
			if (y > MAX_BIN || y < MIN_BIN) warning_histoS2 = true;
		}
	}

	th_3->SetFillColor(5);//giallo
	th_4->SetFillColor(1);//nero

	TCanvas* c_6 = new TCanvas("c_6","c_6",0,0,700,500);
	c_6->cd();

	th_3->Draw();
	th_4->Draw("SAME");

	if (warning_histoS2 == true) cout << "WARNING: Almeno una misura esce dai limiti imposti all'istogramma giorno-notte di S2" << endl;

	//cout giorno-notte
	cout << "\n-------------------" << endl;
	cout << "S1 ha registrato: \n"<<
			"col sole \t media = " << th_1 -> GetMean() << " conteggi all'ora\n" 
		 << "\t\t equivalenti a " << (th_1 -> GetMean()) / 60 << " al minuto." << endl;
	cout << "di notte \t media = " << th_2 -> GetMean() << " conteggi all'ora\n" 
		 << "\t\t equivalenti a " << (th_2 -> GetMean()) / 60 << " al minuto." << endl;
	cout << "S2 ha registrato: \n" <<
			"col sole \t media = " << th_3 -> GetMean() << " conteggi all'ora\n" 
		 << "\t\t equivalenti a " << (th_3 -> GetMean()) / 60 << " al minuto." << endl;
	cout << "di notte \t media = " << th_4 -> GetMean() << " conteggi all'ora\n" 
		 << "\t\t equivalenti a " << (th_4 -> GetMean()) / 60 << " al minuto." << endl;

	//----------------------------------------------------------
	//						EFFICIENZA
	//----------------------------------------------------------

	//efficienza S1
	TGraph* gr_7= new TGraph;
	for(int i = 1; i < conteggiS1.size(); i++){
		double x = i;
		int N = gr_7 -> GetN();
		double y = (tripleS1[i] - tripleS1[i-1]) / (doppie[i] - doppie[i-1]);
		gr_7 -> SetPoint(N,x,y);
	}

	gr_7->SetTitle("Efficienza di S1 nel tempo");
	gr_7->SetMarkerColor(4);//blu
	gr_7->SetMarkerSize(1);
	gr_7->SetMarkerStyle(8);

	TCanvas* c_7= new TCanvas("c_7","c_7",0,0,700,500);
	c_7->cd();

	gr_7->Draw("AP");

	//efficienza S2
	TGraph* gr_8= new TGraph;
	for(int i = 1; i < conteggiS2.size(); i++){
		double x = i;
		int N = gr_8 -> GetN();
		double y = (tripleS2[i] - tripleS2[i-1]) / (doppie[i] - doppie[i-1]);
		gr_8 -> SetPoint(N,x,y);
	}

	gr_8->SetTitle("Efficienza di S2 nel tempo");
	gr_8->SetMarkerColor(4);//blu
	gr_8->SetMarkerSize(1);
	gr_8->SetMarkerStyle(8);

	TCanvas* c_8= new TCanvas("c_8","c_8",0,0,700,500);
	c_8->cd();

	gr_8->Draw("AP");

	//cout efficienza
	cout << "\n-------------------" << endl;
	cout << "Efficienza media di S1: " << tripleS1[tripleS1.size() - 1] / doppie[doppie.size() - 1]  << endl;
	cout << "Efficienza media di S2: " << tripleS2[tripleS2.size() - 1] / doppie[doppie.size() - 1]  << endl;


	myApp.Run();
	return 0;
}


bool leggi_dati(vector<double> & conteggiS1, vector<double> & conteggiS2, vector<double> & doppie, vector<double> & tripleS1, vector<double> & tripleS2, int argc, char** argv){
	if(argc < 2){
		cout << "Digita il nome del file di dati" << endl;
		return false;
	}
	double scal1, scal2, scal3, scal4, scal5, scal6, scal7, scal8, scal9, scal10, scal11, scal12;
	ifstream Infile;
	Infile.open(argv[1], fstream::in);
	if (Infile.good () == false){
		cout << "Errore nell'apertura dei file" << endl;
		return false;
	}
	cout << "Leggo file di dati " << argv[1] << endl;
	int riga = 1;
	while(true){
		Infile >> scal1 >> scal2 >> scal3 >> scal4 >> scal5 >> scal6 >> scal7 >> scal8 >> scal9 >> scal10 >> scal11 >> scal12;
		
		conteggiS1.push_back(scal2);
		conteggiS2.push_back(scal4);
		doppie.push_back(scal5);
		tripleS1.push_back(scal7);
		tripleS2.push_back(scal8);
		if (Infile.eof() == true) break;
	}
	Infile.close();
	return true;
}