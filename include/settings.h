//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

#ifndef settings_h_
#define settings_h_

#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <map>

#include "pugixml/pugixml.hpp"
#include "pugixml/pugixml.cpp"

struct modulesettings{
	int HV;
	int low_ROI;
	int high_ROI;
};


class settings{
public:
	settings(std::string file);
	~settings();
	
	void read_process_settings();
	
	void read_plot_settings();
	
	//processors
	bool calibrate;
	bool raw_data;
	bool pedenoise;
	bool noisemap;
	bool calibrate_data;
	bool clustering;
	
	//general
	std::string filename;
	
	//config
	std::string configfile;
	int run;
	int cal_run;
	std::string runstring;
	std::string cal_runstring;
	std::string datapath;
	std::string cal_datapath;
	std::string result_datapath;
	
	//calibration
	int nominal_signal;
	
	//raw_dat
	int max_events;
	
	//pedenoise
	bool cm;
	int cm_group;
	int n_discarded_strips;
	double hitrejectioncut;
	std::string algorithm;
	int cm_loops;
	
	//noisemap
	double reject_cut;
	int stripgroup;
	
	//data
	int htr_events;
	int cluwindow;
	double seed;
	
	//cluster
	double clu_seed;
	double clu_neighbor;
	double clu_total;
	
	//plots
	bool beamprofile;
	bool noiseprofile;
	bool clusterwidth;
	bool eta;
	bool signal;
	bool snr;
	std::map<std::string, modulesettings> modulemap;
	bool in_roi(int stripnumber, std::string name);
};

#endif