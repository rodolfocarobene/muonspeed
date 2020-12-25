
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector> 

using namespace std;

int main(int argc, char *argv[]){
  ofstream myfile;
  myfile.open ("example.txt");
  for(int i = 0; i< 100000; i++){
  	myfile << "Numero: " << i << "\n";
  	cout << "Numero: " << i << endl;
  }
  myfile.close();

}