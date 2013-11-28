//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#include "pedenoise.h"


	
void pedenoise_data::read_data (std::string eventdata_filename) 
{
	this->pedenoise_eventdata_file = TFile::Open(TFile::AsyncOpen(eventdata_filename.c_str(),"OPEN"));
	if(!this->pedenoise_eventdata_file) std::cerr << "Could not open Pedenoise Event Data file" << std::endl;
	else
	{
		this->pedenoise_data = (TTree*) pedenoise_eventdata_file->Get("pedenoisedata_tree");
		if(!this->pedenoise_data) std::cerr << "Could not open Pedenoise Event Data Tree" << std::endl;
		else 
		{
			TBranch* eventno_br = 0;
			TBranch* napv_br = 0;
			TBranch* adc_br = 0;
			this->pedenoise_data->SetBranchAddress("event_number", &this->event_number, &eventno_br);
			this->pedenoise_data->SetBranchAddress("n_apv", &this->n_apv, &napv_br);
			this->pedenoise_data->SetBranchAddress("adc_vector", &this->adc_vector,&adc_br);
		}
	}
}

unsigned int pedenoise_data::get_n_entries()
{
    if (this->pedenoise_data == NULL){
        std::cerr << "[ERROR] No PedeNoise Data available!" << std::endl;
        return 0;
    }
    return static_cast<unsigned int>(this->pedenoise_data->GetEntries());
}

void pedenoise_data::get_data_entry(int i_entry)
{
	if (this->pedenoise_data != NULL)
	{
		this->adc_vector = NULL;
		pedenoise_data->GetEntry(i_entry);
	}
}



pedenoise::pedenoise(config* theconfig)
{
	this->aconfig = theconfig;
}

pedenoise::~pedenoise() {
	// delete this->pedenoise_file;
	// delete this->pedenoise_tree;
	// delete this->aconfig;
// 	delete this->pedenoiseprofile;
	// delete this->pedestals;
// 	delete this->noise;
}
	
void pedenoise::calculate_pedenoise(bool _cm, int _cm_group, int _n_discard_strips, double _hitrejectcut, int _algorithm, int _n_cm_loops)
{
	this->cm = _cm;
	this->cm_group = _cm_group;
	this->n_discard_strips = _n_discard_strips;
	this->hitrejectcut = _hitrejectcut;
	this->algorithm = _algorithm;
	this->n_cm_loops = _n_cm_loops;
	
	TGraph* noisegraph = NULL;
	
	std::string datafilename = this->aconfig->resultpath + "run" + this->aconfig->runstring + "_pedenoise_data.root";
	
	pedenoise_data mydata;
	mydata.read_data(datafilename);
	
	TProfile* prelimiprofile = new TProfile("preliminaryProfile","preliminaryProfile",this->aconfig->max_apv * APV_CHANNELS,0.5,(this->aconfig->max_apv * APV_CHANNELS) + 0.5);
	prelimiprofile->SetErrorOption("s");
	TProfile* pedeprofile = new TProfile("pedeProfile","pedeProfile",this->aconfig->max_apv * APV_CHANNELS,0.5,(this->aconfig->max_apv * APV_CHANNELS) + 0.5);
	pedeprofile->SetErrorOption("s");
	std::cout << "[PEDENOISE] Running preliminary loop to estimate noise for hit rejection!" << mydata.get_n_entries() << std::endl;
	
	//prelim Noise loop
	for (unsigned int i=0; i<mydata.get_n_entries();i++)
	{
		mydata.get_data_entry(i);
        
		for (int strip=0; strip < mydata.n_apv * APV_CHANNELS; strip++) 
		{
			prelimiprofile->Fill(strip+1,mydata.adc_vector->at(strip));
		}
	}
	
	std::vector<double> prelimnoise(mydata.n_apv * APV_CHANNELS,0);
	std::vector<double> prelimpede(mydata.n_apv * APV_CHANNELS,0);
	
	
	
	for (int strip=0;strip < mydata.n_apv * APV_CHANNELS ;strip++)
	{
		prelimnoise.at(strip) = prelimiprofile->GetBinError(strip+1);
		prelimpede.at(strip) = prelimiprofile->GetBinContent(strip+1);
	}
	delete prelimiprofile;
	std::cout << "[PEDENOISE] Done estimating Noise. The hit rejection cut is " << hitrejectcut << " x noise!" << std::endl;
	
	//Pedestal & Noise Loop
	std::cout << "[PEDENOISE] Running loop to calculate Pedestal and Noise!" << std::endl;
    
	for (unsigned int i=0; i<mydata.get_n_entries();i++) //event loop
	{
		mydata.get_data_entry(i);
        
		for (int strip=0; strip < mydata.n_apv * APV_CHANNELS; strip++) 
		{     
			if (fabs(mydata.adc_vector->at(strip) - prelimpede.at(strip)) < hitrejectcut * prelimnoise.at(strip)) //not hit (in both "directions")
			{
				pedeprofile->Fill(strip+1, mydata.adc_vector->at(strip));
			}
		}
	}
	
	std::vector<double> anoise(mydata.n_apv * APV_CHANNELS,0);
	std::vector<double> apedestal(mydata.n_apv * APV_CHANNELS,0);
	
	for (int strip=0;strip < mydata.n_apv * APV_CHANNELS ;strip++)
	{
		anoise.at(strip) = pedeprofile->GetBinError(strip+1);
		apedestal.at(strip) = pedeprofile->GetBinContent(strip+1);
	}
	std::cout << "[PEDENOISE] Done with calculating Pedestal and Noise!" << std::endl;
	
	//IF COMMON MODE CORRECTION
	if (cm) 
	{
		delete pedeprofile;
		for (int cm_iter = 0; cm_iter < n_cm_loops; cm_iter++)
		{
			std::cout << "[PEDENOISE] Running loop Number " << cm_iter+1 << " for Common Mode Correction!" << std::endl;
        
			TProfile* cm_pedeprofile = new TProfile("cm_pedestalProfile","Common Mode Pedestal Profile",this->aconfig->max_apv * APV_CHANNELS,0.5,(this->aconfig->max_apv * APV_CHANNELS) + 0.5);
			cm_pedeprofile->SetErrorOption("s");
			
			for (unsigned int i=0; i<mydata.get_n_entries();i++) //event loop
			{
				mydata.get_data_entry(i);
				// a little sanity check
        		if ((this->aconfig->max_apv * APV_CHANNELS) % cm_group != 0)
				{
					std::cout << "[WARNING] The total strip number  divided by the selected CM Group Size is not an integer!" << std::endl << "[WARNING] Setting CM Group to default value of 32!" << std::endl;
					cm_group = 32;
				}
				
				int n_regions = (this->aconfig->max_apv * APV_CHANNELS)/cm_group;
				
				std::vector<double> cm(n_regions,0);
				std::vector< std::vector<double> > cm_vectors;
				
				for (int help=0; help<n_regions; help++)
				{
					std::vector<double> dummy;
					cm_vectors.push_back(dummy);
				}
				
				for (int strip=0; strip < mydata.n_apv * APV_CHANNELS; strip++) 
				{
					if (fabs(mydata.adc_vector->at(strip) - apedestal.at(strip)) < hitrejectcut * anoise.at(strip)) // not hit
					{     
						int region = strip/cm_group;
						if (region >= 0 && region < n_regions) //strip for common mode calculation, include borders
						{
							cm_vectors.at(region).push_back(mydata.adc_vector->at(strip) - apedestal.at(strip));
						}
					}
				}
				// now i have a filled vector
				for (unsigned int region=0;region<cm_vectors.size();region++)
				{
					std::sort(cm_vectors.at(region).begin(), cm_vectors.at(region).end());
					
					// a little sanity check for the n_strips to throw away
					if (cm_vectors.at(region).size() < 20) n_discard_strips = 3;
					
					for (unsigned int strip = 0; strip < cm_vectors.at(region).size(); strip++)
					{
						if (static_cast<int>(strip) < n_discard_strips || static_cast<int>(strip) >= static_cast<int>(cm_vectors.at(region).size()) - n_discard_strips)
						{
							cm_vectors.at(region).erase(cm_vectors.at(region).begin() + strip);
						}
					}
					// now decide on algorithm
					if (algorithm == 0) //MEAN
					{
						for (unsigned int str = 0; str < cm_vectors.at(region).size(); str++)
						{
							cm.at(region) += cm_vectors.at(region).at(str);
						}
						cm.at(region) /= cm_vectors.at(region).size();
					}
					else if (algorithm == 1) //MEDIAN
					{
						int half = cm_vectors.at(region).size()/2;
						if (half != 0) cm.at(region) = cm_vectors.at(region).at(half);
						else cm.at(region) = 0;
					}
				}
				
				// I have calculated the CM for this event -> now fill another profile
				
				for (int strip=0; strip < mydata.n_apv * APV_CHANNELS; strip++) 
				{
					double cm_corrected = mydata.adc_vector->at(strip) - cm.at(static_cast<int>(strip/cm_group));
					
					if (fabs(cm_corrected - apedestal.at(strip)) < hitrejectcut * anoise.at(strip))
					{
						cm_pedeprofile->Fill(strip+1,cm_corrected);
					}
				}
			}
			//filled with cm corrected values
			TGraph* agraph = new TGraph();
			
			for (int strip=0;strip < mydata.n_apv * APV_CHANNELS ;strip++)
			{
				anoise.at(strip) = cm_pedeprofile->GetBinError(strip+1);
				apedestal.at(strip) = cm_pedeprofile->GetBinContent(strip+1);
				agraph->SetPoint(agraph->GetN(), strip,cm_pedeprofile->GetBinError(strip+1));
			}
			if (cm_iter < n_cm_loops - 1)
			{
				delete cm_pedeprofile;
				delete agraph;
			}
			else
			{
				this->pedenoiseprofile = cm_pedeprofile;
				noisegraph = agraph;
			}
		} // end of cm iterations loop
		std::cout << "[PEDENOISE] Im done calculating the common mode!" << std::endl;
	}
	
	
	else this->pedenoiseprofile = pedeprofile;

	this->aconfig->resultfile->WriteObject(this->pedenoiseprofile, "Pedestal Profile");
	this->aconfig->resultfile->WriteObject(noisegraph, "Noise Graph");
	
	// this->noise = &anoise;
// 	this->pedestals = &apedestal;
	
	
	//save to TFile
	std::string filename =  this->aconfig->resultpath + "run" + this->aconfig->runstring + "_pedenoise.root";
	TFile* pedenoise_file = TFile::Open(TFile::AsyncOpen(filename.c_str(),"RECREATE"));
	// at a later stage delete the _pedenoise_data.root file
	if (!pedenoise_file) std::cerr << "[ERROR] Cannot create PedeNoise File!" << std::endl;
	else 
	{
		TTree* pedenoise_tree = new TTree("PedeNoise","Pedestal and Noise Values");
		pedenoise_tree->Branch("Pedestals", &apedestal);
		pedenoise_tree->Branch("Noise", &anoise);
		pedenoise_tree->Branch("cm_group", &this->cm_group);
		pedenoise_tree->Branch("n_discard_strips", &this->n_discard_strips);
		pedenoise_tree->Branch("hitrejectcut", &this->hitrejectcut);
		pedenoise_tree->Branch("algorithm", &this->algorithm);
		
		
		pedenoise_tree->Fill();
		pedenoise_file->Write();
		pedenoise_file->Close();
		std::cout << "[PEDENOISE] Pedestal and Noise File successfully created!" << std::endl;
	}
}

void pedenoise::read_pedenoise()
{
	// pedestals = new std::vector<double>;
// 	noise = new std::vector<double>;
	this->pedestals = NULL;
	this->noise = NULL;
	std::string filename =   this->aconfig->resultpath + "run" + this->aconfig->runstring + "_pedenoise.root";
	TFile* pedenoise_file = TFile::Open(TFile::AsyncOpen(filename.c_str(),"OPEN"));
	if(!pedenoise_file) std::cerr << "Could not open Pedenoise file" << filename << std::endl;
	else
	{
		TTree* pedenoise_tree = (TTree*) pedenoise_file->Get("PedeNoise");
		if(!pedenoise_tree) std::cerr << "Could not open Pedenoise Tree" << std::endl;
		else 
		{
			TBranch* noisebranch = 0;
			TBranch* pedestalbranch = 0;
			TBranch* cm_group_branch = 0;
			TBranch* n_discard_branch = 0;
			TBranch* hitreject_branch = 0;
			TBranch* algorithm_branch = 0;
			
			pedenoise_tree->SetBranchAddress("Pedestals", &this->pedestals, &pedestalbranch);
			pedenoise_tree->SetBranchAddress("Noise", &this->noise, &noisebranch);
			
			pedenoise_tree->SetBranchAddress("cm_group", &this->cm_group, &cm_group_branch);
			pedenoise_tree->SetBranchAddress("n_discard_strips", &this->n_discard_strips, &n_discard_branch);
			pedenoise_tree->SetBranchAddress("hitrejectcut", &this->hitrejectcut, &hitreject_branch);
			pedenoise_tree->SetBranchAddress("algorithm", &this->algorithm, &algorithm_branch);
			int i_entry = 0;
			pedenoise_tree->GetEntry(i_entry);
			std::cout << "Successfully read Pedestal and Noise Data!" << std::endl;
		}
	}
}
