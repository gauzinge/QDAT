//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#ifndef data_h_
#define data_h_

#include <iostream>
#include <vector>
#include <map>
#include <set>

#include <TH2.h>
#include <TH1.h>
#include <TObject.h>

#include "definitions.h"
#include "config.h"
#include "read_raw.h"
#include "eventdata.h"
#include "pedenoise.h"
#include "langaus.h"


class data{
public:
	data(config*);
	~data();
	
	// common data
	int event;
	int subevt;
	std::vector<double>* adc;
	
	void process(int htr_events, int cluwindow, double seedcut);
	
	void load_tree();
	unsigned int get_n_events();
	void get_event(int i_event);
	
private:
	config* aconfig;
	pedenoise apedenoise;
	eventdata myeventdata;
	std::vector< std::vector<double> > cal_subevents;
	TH2D* subevent_histo;
	std::vector<double> help_adc;
	
	void calibrate_event(int i_event); //every subevent
	void fill_subevent_histo(int clu_window, double seedcut); // searches for seeds in each event and puts them in histogram
	int get_subevent(); // fits the landau slices and finds the maximum
		
	void init_datafile();
	void write_caldata(int i_entry); // loops over all events, calibrates one subevent and writes it to tree
	void close_file();

	TFile* datafile;
	TTree* datatree;
};

#endif