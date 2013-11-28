//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#ifndef noisemap_h_
#define noisemap_h_

#include <iostream>
#include <vector>

#include "config.h"
#include "pedenoise.h"
#include "definitions.h"

#include "TFile.h"
#include "TTree.h"


class noisemap{
public:
	noisemap(config* theconfig);
	~noisemap();
	double noisecut;
	int stripgroup;
	std::vector<int>* noise_strips;
	void load_noisemap();
	bool strip_noisy(int strip);
	void find_noisestrips(double noisecut, int stripgroup);
private:
	config* aconfig;
	pedenoise apedenoise;
	std::vector<int> help_vec;
};

#endif