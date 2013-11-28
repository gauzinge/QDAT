//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#ifndef cluster_h_
#define cluster_h_

#include <iostream>
#include <vector>
#include <list>
#include <string>

#include "definitions.h"
#include "config.h"
#include "pedenoise.h"
#include "noisemap.h"
#include "data.h"

#include "TFile.h"
#include "TTree.h"

class cluster{
public:
	std::string modulename;
	int event;
	int first_strip;
	int seed_strip; // relative to first
	double cog;
	double signal;
	double noise;
	double seed_noise;
	double eta;
	std::vector<double>* str_signals;
	// std::vector<double>* str_noise;
	
	cluster(config* theconfig);
	~cluster();
	//METHODS TO READ
	void read_file();
	int get_n_entries();
	void get_entry(int i_entry);
	
	//METHODS TO CREATE COLLECTION
	void find_clusters(double seed, double neighbor, double total);
private:
	config* aconfig;
	pedenoise apedenoise;
	noisemap anoisemap;
	data adata;
	std::vector<double> help_str_signals; //convert to vector before writing to tree
	std::string* help;	
	
	TFile* clusterfile;
	TTree* clustertree;
};

#endif