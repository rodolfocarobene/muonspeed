//c++ -o tensione tensione.cpp `root-config --cflags --glibs`

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector> 

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"

using namespace std;

int main (int argc, char** argv)
{
	TApplication myApp("myApp",NULL,NULL);
	

	TGraph* Riv_1= new TGraph("data.txt","%lg %lg %*lg");
	Riv_1->SetName("Rivelatore 1");
	Riv_1->SetMarkerColor(2);//rosso
	Riv_1->SetMarkerSize(3);
	Riv_1->SetMarkerStyle(3);
		
	
	TCanvas* c1 = new TCanvas("c1","c1",0,0,700,500);
	c1->cd();

	Riv_1->Draw();
	c1->Modified();
	c1->Update();
		

	myApp.Run();
	return 0;
}

