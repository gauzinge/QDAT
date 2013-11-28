//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#ifndef calibration_h_
#define calibration_h_

#include <iostream>
#include <vector>
#include <map>

#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TMultiGraph.h>

#include "config.h"

class calibration{
public:
	calibration(config* theconfig);
	~calibration();
	void get_calfactors(int nominal_signal);
	void load_calibration();
	std::vector<double>* calfactor;
	int nominal;
private:
	config* aconfig;
	std::vector<double> help_calibfactor;
	std::vector<TMultiGraph*> apv_curves;
};

#endif