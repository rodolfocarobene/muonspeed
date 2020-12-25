// c++ -o unite unite.cpp

// ./unite /mnt/c/Users/Rodolfo/Desktop/calibrazione/2/datiADC0.txt /mnt/c/Users/Rodolfo/Desktop/calibrazione/2/datiADC1.txt /mnt/c/Users/Rodolfo/Desktop/calibrazione/2/datiTDC.txt output.txt

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector> 

using namespace std;

#define CHAN_ADC0 1
#define CHAN_ADC1 1
#define CHAN_TDC  1

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
		//Infile >> scal1 >> scal2 >> scal3 >> scal4 >> scal5 >> scal6 >> scal7 >> scal8;
		Infile >> scal1;
		switch(channel){
			case 1: 
				ampiezzas1.push_back(scal1);
				break;
			case 2: 
				ampiezzas1.push_back(scal2);
				break;
			case 3: 
				ampiezzas1.push_back(scal3);
				break;
			case 4: 
				ampiezzas1.push_back(scal4);
				break;
			case 5: 
				ampiezzas1.push_back(scal5);
				break;
			case 6: 
				ampiezzas1.push_back(scal6);
				break;
			case 7: 
				ampiezzas1.push_back(scal7);
				break;
			case 8: 
				ampiezzas1.push_back(scal8);
				break;	
			default:
				cout << "\nLa colonna nel define e' errata" << endl;
				return 1;
				break;
		}
		if (Infile.eof() == true) break;
	}
	Infile.close();
	cout << "Numero dati: " << ampiezzas1.size() << endl;
	return true;
}

bool scrivi_dati(vector<double> & adc0, vector<double> & adc1, vector<double> & tdc, char * file){
	cout << "Apro il file: " << file << endl;
	ofstream myfile;
  	myfile.open (file);
  	for(int i = 0; i< adc0.size(); i++){
  		myfile << tdc[i] << " " << adc0[i] << " " << adc1[i] << endl;
  	}
  	myfile.close();
	return true;
}

int main(int argc, char *argv[]){
	if(argc < 5){
		cout << "Dati: adc0 - adc1 - tdc - output" << endl;
		return 1;
	}
	vector<double> adc0;
	vector<double> adc1;
	vector<double> tdc;
	if(!(leggi_dati(adc0, CHAN_ADC0, argv[1]))) return 1;
	if(!(leggi_dati(adc1, CHAN_ADC1, argv[2]))) return 1;
	if(!(leggi_dati(tdc, CHAN_TDC, argv[3]))) return 1;
	cout << "Done" << endl;

	if(!(scrivi_dati(adc0, adc1, tdc, argv[4]))) return 1;

	return 0;
}