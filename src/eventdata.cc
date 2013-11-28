//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

#include "eventdata.h"

eventdata::eventdata(config* theconfig)
{
	this->aconfig = theconfig;
	this->raw_event_datafile = NULL;
}

eventdata::~eventdata()
{
	
}

void eventdata::read_rawdata()
{
	// this->myrawdata = new raw_event_data;
	
	std::string filename =   this->aconfig->resultpath + "run" + this->aconfig->runstring + "_adcdata.root";
	this->raw_event_datafile = TFile::Open(TFile::AsyncOpen(filename.c_str(),"OPEN")); 
	if(!this->raw_event_datafile) std::cerr << "Could not open raw event data file" << std::endl;
	else
	{
		this->raw_event_datatree = (TTree*) raw_event_datafile->Get("data_tree");
		if (!this->raw_event_datatree) std::cerr << "Could not open raw event data Tree" << std::endl;
		else 
		{
			TBranch* eventbranch = 0;
			TBranch* n_subeventbranch = 0;
			TBranch* subeventbranch_0 = 0;
			TBranch* subeventbranch_1 = 0;
			TBranch* subeventbranch_2 = 0;
			TBranch* subeventbranch_3 = 0;
			TBranch* subeventbranch_4 = 0;
			TBranch* subeventbranch_5 = 0;
			
			this->raw_event_datatree->SetBranchAddress("event_number",&myrawdata.event,&eventbranch);
			this->raw_event_datatree->SetBranchAddress("n_subevents", &myrawdata.n_subevents,&n_subeventbranch);
			
			this->raw_event_datatree->SetBranchAddress("adc_vector_0",&myrawdata.subevent0,&subeventbranch_0);
			this->raw_event_datatree->SetBranchAddress("adc_vector_1",&myrawdata.subevent1,&subeventbranch_1);
			this->raw_event_datatree->SetBranchAddress("adc_vector_2",&myrawdata.subevent2,&subeventbranch_2);
			this->raw_event_datatree->SetBranchAddress("adc_vector_3",&myrawdata.subevent3,&subeventbranch_3);
			this->raw_event_datatree->SetBranchAddress("adc_vector_4",&myrawdata.subevent4,&subeventbranch_4);
			this->raw_event_datatree->SetBranchAddress("adc_vector_5",&myrawdata.subevent5,&subeventbranch_5);
		}
	}
}

unsigned int eventdata::get_n_entries()
{
    if (raw_event_datatree == NULL){
        std::cerr << "[ERROR] No raw event data available!" << std::endl;
        return 0;
    }
    return static_cast<unsigned int>(raw_event_datatree->GetEntries());
}

void eventdata::get_entry(int i_entry)
{
	if (this->raw_event_datatree != NULL)
	{
		this->myrawdata.subevent0 = NULL;
		this->myrawdata.subevent1 = NULL;
		this->myrawdata.subevent2 = NULL;
		this->myrawdata.subevent3 = NULL;
		this->myrawdata.subevent4 = NULL;
		this->myrawdata.subevent5 = NULL;
		
		raw_event_datatree->GetEntry(i_entry);
		
		// this->myrawdata.subevent0_data = *this->myrawdata.subevent0;
// 		this->myrawdata.subevent1_data = *this->myrawdata.subevent1;
// 		this->myrawdata.subevent2_data = *this->myrawdata.subevent2;
// 		this->myrawdata.subevent3_data = *this->myrawdata.subevent3;
// 		this->myrawdata.subevent4_data = *this->myrawdata.subevent4;
// 		this->myrawdata.subevent5_data = *this->myrawdata.subevent5;
		
		this->myrawdata.subevent_data.clear();
		this->myrawdata.subevent_data.push_back(*this->myrawdata.subevent0);
		this->myrawdata.subevent_data.push_back(*this->myrawdata.subevent1);
		this->myrawdata.subevent_data.push_back(*this->myrawdata.subevent2);
		this->myrawdata.subevent_data.push_back(*this->myrawdata.subevent3);
		this->myrawdata.subevent_data.push_back(*this->myrawdata.subevent4);
		this->myrawdata.subevent_data.push_back(*this->myrawdata.subevent5);
	}
}
