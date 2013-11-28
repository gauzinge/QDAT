//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#ifndef eventdata_h_
#define eventdata_h_


#include "definitions.h"
#include "config.h"
#include "read_raw.h"
#include "pedenoise.h"
#include "langaus.h"

#include <TFile.h>
#include <TTree.h>

#include <iostream>
#include <vector>

class eventdata{
public:
	raw_event_data myrawdata;
	eventdata(config* theconfig);
	~eventdata();
	void read_rawdata(); // call all of me in process
	unsigned int get_n_entries();
	void get_entry(int i_entry); // moves the subevent vectors back into a vector of vectors
private:
	config* aconfig;
	TFile* raw_event_datafile;
	TTree* raw_event_datatree;
	
};



#endif