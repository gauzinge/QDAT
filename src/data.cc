//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#include "data.h"


data::data(config* theconfig) : apedenoise(theconfig) , myeventdata(theconfig)
{
	this->aconfig = theconfig;
	this->apedenoise.read_pedenoise();
	
	this->subevent_histo = new TH2D("subevent_histo","Subevent Histogram",6,0.5,6.5,60,0,120);
}

data::~data()
{
    if (this->datafile!=NULL){
        this->datafile->Close();
        delete this->datafile;
    }
	// if (this->read_datafile!=NULL){
// 		this->read_datafile->Close();
// 		delete this->read_datafile;
// 	}
}

void data::process (int htr_events, int cluwindow, double seedcut) 
{	
	std::cout << std::endl << "********************************************" << std::endl;
	std::cout << "[HTR] Running Hit Time Reconstruction for a set of " << htr_events << " events!" << std::endl;

	this->myeventdata.read_rawdata();
	
	int maxevents; 
	if (htr_events > static_cast<int>(this->myeventdata.get_n_entries())) maxevents = static_cast<int>(this->myeventdata.get_n_entries());
	else maxevents = htr_events;
	for (int i = 0; i < maxevents; i++)
	{
		this->myeventdata.get_entry(i);
		this->calibrate_event(i);			
		this->fill_subevent_histo(cluwindow, seedcut);
	}
		
	this->subevt = this->get_subevent();
	std::cout << "********************************************" << std::endl 
		<< "[HTR] Calibrating Subevent " << this->subevt << std::endl;
	
	this->init_datafile();	
	for (unsigned int i = 0; i < this->myeventdata.get_n_entries(); i++)
	{
		this->myeventdata.get_entry(i);
		this->write_caldata(i);
	}
	this->close_file();
	std::cout << "********************************************" << std::endl;
}

void data::load_tree()
{
	this->adc = NULL;
	
	std::string filename = this->aconfig->resultpath + "run" + this->aconfig->runstring + "_caldata.root";
	
	this->datafile = TFile::Open(TFile::AsyncOpen(filename.c_str(),"OPEN"));
	if (!this->datafile) std::cerr << "Could not open data file!" << std::endl;
	else
	{
		this->datatree = (TTree*) this->datafile->Get("caldata");
		if (!this->datatree) std::cerr << "Could not load data TTree!" << std::endl;
		else 
		{
			TBranch* eventbranch = 0;
			TBranch* subeventbranch = 0;
			TBranch* adcbranch = 0;
			
			this->datatree->SetBranchAddress("event", &this->event,&eventbranch);
			this->datatree->SetBranchAddress("subevent", &this->subevt, &subeventbranch);
			this->datatree->SetBranchAddress("adc", &this->adc, &adcbranch);
		}
	}
}

unsigned int data::get_n_events()
{
    if (this->datatree == NULL){
        std::cerr << "[ERROR] No event data available!" << std::endl;
        return 0;
    }
    return static_cast<unsigned int>(this->datatree->GetEntries());
}

void data::get_event(int i_event)
{
	if (this->datatree != NULL)
	{
		this->datatree->GetEntry(i_event);
	}
}

void data::calibrate_event(int i_event)
{
	//create a vector of vectors of doubles that has the same dimensions as myrawdata.subevent_data, initialize with 0
	for (int i = 0; i < 6; i++)
	{
		std::vector<double> dummy (this->myeventdata.myrawdata.subevent_data.at(0).size(),0);
		this->cal_subevents.push_back(dummy);
	}
	
	for (int sub = 0; sub < this->myeventdata.myrawdata.n_subevents; sub++)
	{
		// a little sanity check
		if ((this->aconfig->max_apv * 128) % this->apedenoise.cm_group != 0)
		{
			std::cout << "[WARNING] The total strip number  divided by the selected CM Group Size is not an integer!" << std::endl << "[WARNING] Setting CM Group to default value of 32!" << std::endl;
			this->apedenoise.cm_group = 32;
		}
	
		int n_regions = (this->aconfig->max_apv * 128)/this->apedenoise.cm_group;
	
		std::vector<double> cm(n_regions,0);
		std::vector< std::vector<double> > cm_vectors;
	
		for (int help=0; help<n_regions; help++)
		{
			std::vector<double> dummy;
			cm_vectors.push_back(dummy);
		}
	
		for (int strip = 0; strip < this->aconfig->max_apv * 128; strip++) 
		{
			if (fabs(this->myeventdata.myrawdata.subevent_data.at(sub).at(strip) - this->apedenoise.pedestals->at(strip)) < this->apedenoise.hitrejectcut * this->apedenoise.noise->at(strip)) // not hit
			{     
				int region = strip/this->apedenoise.cm_group;
				if (region >= 0 && region < n_regions) //strip for common mode calculation, include borders
				{
					cm_vectors.at(region).push_back(this->myeventdata.myrawdata.subevent_data.at(sub).at(strip) - this->apedenoise.pedestals->at(strip));
				}
			}
		}
		// now i have a filled vector
		
		for (unsigned int region=0;region<cm_vectors.size();region++)
		{
			std::sort(cm_vectors.at(region).begin(), cm_vectors.at(region).end());
			
			// a little sanity check for the n_strips to throw away
			if (cm_vectors.at(region).size() < 20) this->apedenoise.n_discard_strips = 3;
			
			for (unsigned int strip = 0; strip < cm_vectors.at(region).size(); strip++)
			{
				if (static_cast<int>(strip) < this->apedenoise.n_discard_strips || static_cast<int>(strip) >= static_cast<int>(cm_vectors.at(region).size()) - this->apedenoise.n_discard_strips)
				{
					cm_vectors.at(region).erase(cm_vectors.at(region).begin() + strip);
				}
			}
			// now decide on algorithm
			if (this->apedenoise.algorithm == 0) //MEAN
			{
				for (unsigned int str = 0; str < cm_vectors.at(region).size(); str++)
				{
					cm.at(region) += cm_vectors.at(region).at(str);
				}
				cm.at(region) /= cm_vectors.at(region).size();
			}
			else if (this->apedenoise.algorithm == 1) //MEDIAN
			{
				int half = cm_vectors.at(region).size()/2;
				if (half != 0) cm.at(region) = cm_vectors.at(region).at(half);
				else cm.at(region) = 0;
			}
		}
		//now calibrate
		for (int strip=0; strip < this->aconfig->max_apv * 128; strip++) 
		{
			this->cal_subevents.at(sub).at(strip) = this->myeventdata.myrawdata.subevent_data.at(sub).at(strip) - this->apedenoise.pedestals->at(strip) - cm.at(static_cast<int>(strip / this->apedenoise.cm_group));
		}
	} // end of subevent loop
	if (i_event % 100 == 0)
	{
		std::cout << "[HTR] " << i_event << " events calibrated! (Event Number: "<< this->myeventdata.myrawdata.event <<")" << std::endl;
	}
}

void data::fill_subevent_histo(int clu_window, double seedcut)
{
	for (unsigned int i = 0; i < this->cal_subevents.size(); i++) //loop over subevents
	{
		for (unsigned int mod = 0; mod < this->aconfig->modules.size(); mod++)
		{
			// int first = this->aconfig->modules.at(mod).first_strip;
			int last = this->aconfig->modules.at(mod).last_strip - clu_window;
			
			int strip = this->aconfig->modules.at(mod).first_strip + clu_window; // so i dont go negative, local strip_number
			int global_strip = this->aconfig->modules.at(mod).global_first_strip + clu_window;
			
			while (strip < last) //to avoid shotting over the module
			{
				if (this->cal_subevents.at(i).at(global_strip)/this->apedenoise.noise->at(global_strip) > seedcut)
				{
					double sig_sum = this->cal_subevents.at(i).at(global_strip);
					// double noise_sum = this->apedenoise.noise->at(global_strip);
				
					for (int up = global_strip + 1; up < global_strip + 3; up++)
					{
						sig_sum += this->cal_subevents.at(i).at(up);
					}
					for (int down = global_strip - 1; down > global_strip - 3; down--)
					{
						sig_sum += this->cal_subevents.at(i).at(down);
					}
					//maybe introduce a total cut check	
					this->subevent_histo->Fill(i+0.5,sig_sum);		
					
					strip += clu_window+1;
					global_strip += clu_window+1;
				}
				else 
				{
					strip++;
					global_strip ++;
				}
			}
		}
	}
}

int data::get_subevent()
{
	std::cout << std::endl << std::endl << "[HTR] Running the Hit Time Reconstruction" ;
	// TCanvas* acanvas = new TCanvas("aa","");
// 	acanvas->cd();
// 	this->subevent_histo->Draw(colz);
	
	this->aconfig->resultfile->WriteObject(this->subevent_histo, "2D Subevent Histogram");
	
	double max_mpv = 0;
	double max_mpv_index = 0;
	for (int i = 0; i < 6; i++)
	{
		double mpv, mpv_error, landau_mpv, landau_mpv_error;
		
		std::stringstream ss;
		ss << "subevent_projection_" << i;
		TH1D* projection = this->subevent_histo->ProjectionY(ss.str().c_str(),i+1,i+1,"eo");
		TF1* fit = langaus(projection, mpv, mpv_error, landau_mpv, landau_mpv_error,0);
		if (mpv > max_mpv)
		{
			max_mpv = mpv;
			max_mpv_index = i;
		}
		this->aconfig->resultfile->WriteObject(projection,ss.str().c_str());
		std::stringstream ps;
		ps << "subevent_projection_" << i << "_fit";
		this->aconfig->resultfile->WriteObject(fit, ps.str().c_str());
		std::cout << " ."; 
	}
	this->aconfig->resultfile->cd();
	this->subevt = max_mpv_index;
	std::cout << std::endl << "[HTR] The subevent with the highest signal is " << this->subevt << std::endl;
	return this->subevt;
}


void data::init_datafile()
{		
	std::string filename = this->aconfig->resultpath + "run" + this->aconfig->runstring + "_caldata.root";
	this->datafile = TFile::Open(filename.c_str(),"RECREATE");
	if (!this->datafile) std::cerr << "[ERROR] Could not open Datafile " << filename << std::endl;
	else 
	{	
		this->datafile->cd();
		this->datatree = new TTree("caldata","Calibrated Data of the best Subevent");
		if (! this->datatree) std::cerr << "[ERROR] Could not create datatree!" << std::endl;
		else
		{
			this->datatree->Branch("event", &this->event);
			this->datatree->Branch("subevent", &this->subevt);
			this->datatree->Branch("adc", &this->help_adc);
		}
	}
}

void data::write_caldata(int i_entry)
{
	std::vector<double> calibrated_event(this->myeventdata.myrawdata.subevent_data.at(0).size(),0);
	
	// a little sanity check
	if ((this->aconfig->max_apv * 128) % this->apedenoise.cm_group != 0)
	{
		std::cout << "[WARNING] The total strip number  divided by the selected CM Group Size is not an integer!" << std::endl << "[WARNING] Setting CM Group to default value of 32!" << std::endl;
		this->apedenoise.cm_group = 32;
	}

	int n_regions = (this->aconfig->max_apv * 128)/this->apedenoise.cm_group;

	std::vector<double> cm(n_regions,0);
	std::vector< std::vector<double> > cm_vectors;
	for (int help=0; help<n_regions; help++)
	{
		std::vector<double> dummy;
		cm_vectors.push_back(dummy);
	}

	for (int strip=0; strip < this->aconfig->max_apv * 128; strip++) 
	{
		if (fabs(this->myeventdata.myrawdata.subevent_data.at(this->subevt).at(strip) - this->apedenoise.pedestals->at(strip)) < this->apedenoise.hitrejectcut * this->apedenoise.noise->at(strip)) // not hit
		{     
			int region = strip/this->apedenoise.cm_group;
			if (region >= 0 && region < n_regions) //strip for common mode calculation, include borders
			{
				cm_vectors.at(region).push_back(this->myeventdata.myrawdata.subevent_data.at(this->subevt).at(strip) - this->apedenoise.pedestals->at(strip));
			}
		}
	}
	// now i have a filled vector
	for (unsigned int region=0;region<cm_vectors.size();region++)
	{
		std::sort(cm_vectors.at(region).begin(), cm_vectors.at(region).end());
		
		// a little sanity check for the n_strips to throw away
		if (cm_vectors.at(region).size() < 20) this->apedenoise.n_discard_strips = 3;
		
		for (unsigned int strip = 0; strip < cm_vectors.at(region).size(); strip++)
		{
			if (static_cast<int>(strip) < this->apedenoise.n_discard_strips || static_cast<int>(strip) >= static_cast<int>(cm_vectors.at(region).size()) - this->apedenoise.n_discard_strips)
			{
				cm_vectors.at(region).erase(cm_vectors.at(region).begin() + strip);
			}
		}
		// now decide on algorithm
		if (this->apedenoise.algorithm == 0) //MEAN
		{
			for (unsigned int str = 0; str < cm_vectors.at(region).size(); str++)
			{
				cm.at(region) += cm_vectors.at(region).at(str);
			}
			cm.at(region) /= cm_vectors.at(region).size();
		}
		else if (this->apedenoise.algorithm == 1) //MEDIAN
		{
			int half = cm_vectors.at(region).size()/2;
			if (half != 0) cm.at(region) = cm_vectors.at(region).at(half);
			else cm.at(region) = 0;
		}
	}
	//now calibrate	
	for (int strip=0; strip < this->aconfig->max_apv * 128; strip++) 
	{
		calibrated_event.at(strip) = this->myeventdata.myrawdata.subevent_data.at(this->subevt).at(strip) - this->apedenoise.pedestals->at(strip) - cm.at(static_cast<int>(strip / this->apedenoise.cm_group));
	}
	
	//now assign
	this->event = this->myeventdata.myrawdata.event;
	this->help_adc = calibrated_event;
	
	if (!this->datafile->IsZombie() && !this->datatree->IsZombie()){
		this->datafile->cd();
		this->datatree->Fill();
	}
	if (i_entry % 100 == 0)
	{
		std::cout <<  "[HTR] " << i_entry << " events calibrated (Event Number " <<  this->myeventdata.myrawdata.event << ")! Flushing File! " <<  std::endl;
		this->datafile->Flush();
	}
}

void data::close_file()
{
	this->datafile->cd();
	this->datatree->Write();
	this->datafile->Write();
	this->datafile->Close();
	std::cout << "[HTR] Successfully finished Calibrating, datafile written!" << std::endl;
}
