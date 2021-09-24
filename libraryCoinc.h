
class evento{
	public:
		evento();
		evento(double tdcval, double adc0val, double adc1val){
			tdc = tdcval;
			adc0 = adc0val;
			adc1 = adc1val;			
		};

		double Get_tdc(){
			return tdc;
		}
		double Get_adc0(){
			return adc0;
		}
		double Get_adc1(){
			return adc1;
		}
		
				//correzione nuova sigmoid
		double Get_tdc_corretto(double tau0, double vth0, double tau1, double vth1){
			return tdc + tau0 * log(vth0/(adc0-vth0))  - 4/tau0 - tau1 * log(vth1/(adc1-vth1)) + 4/tau1;
		}
		double Get_tdc_adc0(double tau0, double vth0, double tau1, double vth1){
			return tdc - tau0 * log(vth0/(adc0-vth0)) + 4/tau0;
		}
		double Get_tdc_adc1(double tau0, double vth0, double tau1, double vth1){
			return tdc - tau1 * log(vth1/(adc1-vth1)) + 4/tau1;
		}

		
	
		double Get_tdc_off(double off0, double off1){
			return - tdc + off0 + off1;
		}
	private:
		double tdc;
		double adc0;
		double adc1;
};

		// funzione sigmoide
double myFun(double *x, double *par){		
	double A = par[0];		//tau
	double B = par[1];		//Vth
	double C = par[2];		//offset
	double D = x[0];
	return C+A*log(B/(D-B));
}




void graficoiniziale(double mins1_graph,double maxs1_graph,TGraphErrors* g_adc0,double mins2_graph,double maxs2_graph,TGraphErrors* g_adc1){
	for(int i = mins1_graph; i <= maxs1_graph; i++){
		double sum = 0;
		double sumt = 0;
		int count = 0;
		double dev = 0;
		for(int j = 0; j < v_eventi.size(); j++){
			double adc0 = v_eventi[j] -> Get_adc0();
			double adc1 = v_eventi[j] -> Get_adc1();

			bool limits = (adc0 > mins1_graph && adc0 < maxs1_graph) && (adc1 > mins2_graph && adc1 < maxs2_graph);

			double tdc = v_eventi[j] -> Get_tdc();
			if(adc0 == i && limits){
				count++;
				sumt = sumt + tdc;
			}
		}
		sumt = sumt / count;
		for(int j = 0; j < v_eventi.size(); j++){
			double adc0 = v_eventi[j] -> Get_adc0();
			double adc1 = v_eventi[j] -> Get_adc1();

			bool limits = (adc0 > mins1_graph && adc0 < maxs1_graph) && (adc1 > mins2_graph && adc1 < maxs2_graph);
			double tdc = v_eventi[j] -> Get_tdc();
			if(adc0 == i && limits){
				dev = dev + pow(tdc-sumt,2)/(count -1);
			}
		}
		dev = sqrt(dev) / sqrt(count);
		if(count == 1) dev = E_TDC;
		int N = g_adc0 -> GetN();
		if(count ) g_adc0 -> SetPoint(N,i,sumt);
		if(count ) g_adc0 -> SetPointError(N,E_ADC,dev);
	}

	for(int i = mins2_graph; i <= maxs2_graph; i++){
		double sum = 0;
		double sumt = 0;
		int count = 0;
		double dev = 0;
		for(int j = 0; j < v_eventi.size(); j++){
			double adc0 = v_eventi[j] -> Get_adc0();
			double adc1 = v_eventi[j] -> Get_adc1();

			bool limits = (adc0 > mins1_graph && adc0 < maxs1_graph) && (adc1 > mins2_graph && adc1 < maxs2_graph);
			double tdc = v_eventi[j] -> Get_tdc();
			if(adc1 == i && limits){
				count++;
				sumt = sumt + tdc;
			}
		}
		sumt = sumt / count;
		for(int j = 0; j < v_eventi.size(); j++){
			double adc0 = v_eventi[j] -> Get_adc0();
			double adc1 = v_eventi[j] -> Get_adc1();

			bool limits = (adc0 > mins1_graph && adc0 < maxs1_graph) && (adc1 > mins2_graph && adc1 < maxs2_graph);
			double tdc = v_eventi[j] -> Get_tdc();
			if(adc1 == i && limits){
				dev = dev + pow(tdc-sumt,2)/(count -1);
			}
		}
		dev = sqrt(dev) / sqrt(count);
		if(count == 1) dev = E_TDC;
		int N = g_adc1 -> GetN();
		if(count != 0) g_adc1 -> SetPoint(N,i,sumt);
		if(count != 0) g_adc1 -> SetPointError(N,E_ADC,dev);
	}
}

void setlimits(TH1D * myhisto, double * min, double * max){	
	double maxcount = 0;
	int maxcountbin = 0;
	for(int i = 0; i < 900; i++){
		int count = myhisto -> GetBinContent(i);
		if (count > maxcount){ 
			maxcountbin = i;
			maxcount = count;
		}
	}

	TF1 * g = new TF1("g","landau",85,95);	
	if(fun1 == true){
		g -> SetParameter (1,17);
	}
	else g -> SetParameter (1,50);
	myhisto -> Fit(g, "M Q");
	g -> SetParLimits(1,maxcountbin-2, maxcountbin+2);
	double mean = g -> GetParameter (1);
	double sigma = g -> GetParameter (2);
	if(fun1 == true){
		*min = mean - 2 * sigma;
		*max = mean + 2 * sigma;
		fun1 = false;
	}
	else{
		*min = mean - 2 * sigma;
		*max = mean + 2 * sigma;
	}
	cout << "media: " << mean << "  sigma: " << sigma << endl;
	return;
}

/*
	//setlimits con FWHM
void setlimits(TH1D * myhisto, double * min, double * max){
	double maxcount = 0;
	int maxcountbin = 0;
	for(int i = 0; i < 900; i++){
		int count = myhisto -> GetBinContent(i);
		if (count > maxcount){ 
			maxcountbin = i;
			maxcount = count;
		}
	}
	maxcount = maxcount/2;

	int minbin;
	int maxbin;

	double delta = 100000;
	for(int i = 0; i < maxcountbin; i++){
		double temp = abs(maxcount - myhisto -> GetBinContent(i));
		if(delta > temp){
			delta = temp;
			minbin = i;
		}
	}

	delta = 100000;
	for(int i = maxcountbin; i < 900 ; i++){
		double temp = abs(maxcount - myhisto -> GetBinContent(i));
		if(delta > temp){
			delta = temp;
			maxbin = i;
		}
	}

	*min = myhisto -> GetBinCenter(minbin);
	*max = myhisto -> GetBinCenter(maxbin);

	cout << "\nmaxcount: " << maxcount << endl;
	cout << "bin: " << minbin << " - " << maxcountbin << " - " << maxbin << endl;
}
*/
