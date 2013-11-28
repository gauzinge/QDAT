//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#ifndef pedenoise_h_
#define pedenoise_h_

#include <TFile.h>
#include <TTree.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TGraph.h>
#include <TObject.h>

#include <string.h>
#include <iostream>
#include <vector>

#include "definitions.h"
#include "read_raw.h"

enum algorithm {MEAN = 0, MEDIAN};

struct pedenoise_data{
	int event_number;
	int n_apv;
	std::vector<int> *adc_vector;
	
    TFile* pedenoise_eventdata_file;
    TTree* pedenoise_data;
	
	void read_data(std::string eventdata_filename);
	unsigned int get_n_entries();
	void get_data_entry(int i_entry);
};

class pedenoise{
private:
	config* aconfig;
	TProfile* pedenoiseprofile;
public:   
	std::vector<double>* pedestals;
	std::vector<double>* noise;
	//Settings
	bool cm;
	int cm_group;
	int n_discard_strips;
	double hitrejectcut;
	int algorithm;
	int n_cm_loops;
	
	//Methods
	pedenoise(config*);
	~pedenoise();
	void calculate_pedenoise(bool _cm = true, int _cm_group = 32, int _n_discard_strips = 3, double _hitrejectcut = 3, int _algorithm = 1, int _n_cm_loops = 2);
	void read_pedenoise();
};

#endif