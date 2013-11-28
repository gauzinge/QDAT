//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#ifndef read_raw_h
#define read_raw_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

#include "config.h"
#include "definitions.h"

#include "TProfile.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

typedef struct { 
         unsigned int header;                 // separator word, always 0xffffffff
         unsigned int eventcnt;               // 32bit event counter, should match with 8bit FADC event number
         unsigned char trg_time;               // measured trigger time (raw format)
         unsigned char subevents;              // number of subevents found within this event
         unsigned char number_apvs;            // number of APVs in this run
         unsigned char number_fadcs;           // number of FADCs in this run
         unsigned int fadcmode;               // current mode (transparent, hit, hit+inputheaders)
         unsigned int tlueventnumber;         // TLU event number (was previously spare1)
         unsigned int spare2;                 // reserved
         unsigned short fadcdatalength[16];    // up to 16 FADCs possible
} T_SAVE_EVENT_HEADER;   

struct raw_event_data{
	int event;
	int n_subevents;
	std::vector< std::vector<int> > subevent_data;
	
	std::vector<int> subevent0_data;
	std::vector<int> subevent1_data;
	std::vector<int> subevent2_data;
	std::vector<int> subevent3_data;
	std::vector<int> subevent4_data;
	std::vector<int> subevent5_data;
	
	std::vector<int>* subevent0;
	std::vector<int>* subevent1;
	std::vector<int>* subevent2;
	std::vector<int>* subevent3;
	std::vector<int>* subevent4;
	std::vector<int>* subevent5;
};

class raw_data{
public:
	raw_data(config* theconfig);
	~raw_data();
	void read_raw(int read_n_events);
	void fill_fifo1_data();
	void process_raw_adc(unsigned int global_event_cnt);
	void init_trees();
	void write_trees();
		
private:
	config aconfig;
	T_SAVE_EVENT_HEADER save_event_header;                   // event header
	unsigned int fadc_rawdata[FADC_MODULES][MAX_FADC_DATA];  // raw data buffer
	unsigned short adc_rawdata[MAX_APV][ADC_SAMPLES];        // raw ADC data buffer
	int channel_lookup[128];
	raw_event_data myraweventdata;
	
	
	//pedenoise collection
    TFile* pedenoise_eventdata_file;
	TTree* pedenoise_data;
	std::string pedefilename;

	//Data collection
    TFile* eventdata_file;
	TTree* datatree;
	std::string datafilename;
	
	//little helpers
	void init_channel_lookup();
	double convert_tdc_value ();
	};	
	
double neco_trgtime2ns(unsigned char trg_time, int sysclock);
	
#endif