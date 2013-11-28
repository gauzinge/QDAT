//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#ifndef config_h_
#define config_h_

#include <TFile.h>
#include <TObject.h>
#include <TDirectory.h>


#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "definitions.h"
#include "settings.h"

struct apv{ // APV object
	int module; // equals module.position
	int position; // APV position
	int global_apvnumber;
	int i2c_adress;
	int fadc_data_offset; //(apv on fadc * 950+1)+1
	int fadc_number;
	int fadc_channel;
	int fadc_clk_delay;
	int first_strip;
};

struct module{ // Module object with several APV objects
	std::string name;
	int position; // module positioin 
	int crate; 
	int n_apv;
	std::vector<apv> chips;
	int sensor_number;
	int sensor_strips;
	int global_first_strip;
	int first_strip;
	int last_strip;
	double pitch;
	double strip_length;
};

class config{ // The global DAQ parameters
public:
	config();
	~config();
	void read_config(settings* mysettings);
	void read_config_silent(settings* mysettings);
	module* find_module(std::string name);
	
	settings* mysettings;
	
	//files and names
	std::string path;
	std::string runstring;
	int run;
	std::string calpath;
	std::string calstring;
	int calrun;
	std::string resultpath;
	
    TFile* resultfile;
	TFile* plotfile;
	
	//global settings
	int clk; //max 25ns
	int frequency;
	int n_adc_samples; //950
	int n_subevents; // 6
	int n_pedenoiseevents; //600
	
	double seed_cut;
	double neighbor_cut;
	double cal_lvl;
	double maxevents;
	
	int max_apv;
	int max_fadc;
	
	//module settings
	int n_modules;
	std::vector<module> modules;

private:
	std::vector<double> read_configline(std::string rawline);
	void read_moduleline(std::string rawline);
	void read_apvline (std::string rawline);
	void read_apvline_2 (std::string rawline);
	void read_sensorline (std::string rawline);
	void read_globalsettings(std::string filename);
	void read_modulesettings(std::string filename);
	// void get_maxevents(std::string textfilename);
};

#endif
