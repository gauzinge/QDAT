//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#include "noisemap.h"

noisemap::noisemap(config* theconfig) : apedenoise(theconfig)
{
	this->aconfig = theconfig;
	this->apedenoise.read_pedenoise();
}

noisemap::~noisemap()
{
	
}
	
void noisemap::find_noisestrips(double noisecut, int stripgroup)
{
	this->noisecut = noisecut;
	std::cout << std::endl << "********************************************" << std::endl;
	std::cout << "[NOISESTRIPS] Searching for noise strips!" << std::endl;
	
	//sanity check
	if ((this->aconfig->max_apv * APV_CHANNELS) % stripgroup != 0)
	{
		std::cout << "[WARNING] The total strip number  divided by the selected Strip Group Size is not an integer!" << std::endl << "[WARNING] Setting Strip Group to default value of 32!" << std::endl;
		stripgroup = 32;
	}
	this->stripgroup = stripgroup;

	int n_regions = (this->aconfig->max_apv * APV_CHANNELS)/stripgroup;
	
	std::vector<double> region_mean(n_regions,0);
	
	for (int strip=0; strip < this->aconfig->max_apv * APV_CHANNELS; strip++) 
	{
		//calculate average noise
		int region = strip/stripgroup;
		if (region >= 0 && region < n_regions) //strip for common mode calculation, include borders
		{			
			region_mean.at(region) += this->apedenoise.noise->at(strip);
		}
	}
	//now I have the sum for every group
	for (int i = 0; i < n_regions; i++)
	{
		region_mean.at(i) /= stripgroup;
	}

	for (int strip=0; strip < this->aconfig->max_apv * APV_CHANNELS; strip++) 
	{
		if (this->apedenoise.noise->at(strip) > noisecut * region_mean.at(static_cast<int>(strip / stripgroup)))
		{
			this->help_vec.push_back(strip);
			std::cout << "Found noisy Strip with global number: " << strip << std::endl;
		}
	}
	std::cout << "********************************************" << std::endl;
	
	std::string filename = this->aconfig->resultpath + "run" + this->aconfig->runstring + "_noisemap.root";
	std::cout << filename << std::endl;
	TFile* noisefile = TFile::Open(TFile::AsyncOpen(filename.c_str(),"RECREATE"));
	if (!noisefile) std::cerr << "Could not create noisemap file!" << std::endl;
	else
	{
		TTree* noisemaptree = new TTree("noisemap","Map of noisy Strips");
		if (!noisemaptree) std::cerr << "Could not create noisemap TTree!" << std::endl;
		else 
		{
			noisemaptree->Branch("noisecut",&this->noisecut);
			noisemaptree->Branch("stripgroup",&this->stripgroup);
			noisemaptree->Branch("noisemap",&this->help_vec);
			noisemaptree->Fill();
		}
		noisefile->Write();
		noisefile->Close();
		std::cout << "[NOISESTRIPS] Successfully wrote Noisemap file!" << std::endl;
	}
}

void noisemap::load_noisemap()
{
	this->noise_strips = 0;
	std::string filename = this->aconfig->resultpath + "run" + this->aconfig->runstring + "_noisemap.root";
	TFile* noisefile = TFile::Open(TFile::AsyncOpen(filename.c_str(),"OPEN"));
	if (!noisefile) std::cerr << "Could not open noisemap file!" << std::endl;
	else
	{
		TTree* noisemaptree = (TTree*) noisefile->Get("noisemap");
		if (!noisemaptree) std::cerr << "Could not load noisemap TTree!" << std::endl;
		else 
		{
			TBranch* noisecutbranch = 0;
			TBranch* stripgroupbranch = 0;
			TBranch* noisemapbranch = 0;
			
			noisemaptree->SetBranchAddress("noisecut",&this->noisecut, &noisecutbranch);
			noisemaptree->SetBranchAddress("stripgroup",&this->stripgroup, &stripgroupbranch);
			noisemaptree->SetBranchAddress("noisemap",&this->noise_strips, &noisemapbranch);
			noisemaptree->GetEntry(0);
		}
		noisefile->Close();
		noisefile = NULL;
		std::cout << "Loaded Collection of Noisy Strips" << std::endl;
		// for (unsigned int i=0; i < this->noise_strips->size();i++)
// 		{
// 			std::cout << "strip No. " << this->noise_strips->at(i) << std::endl;
// 		}
	}
}

bool noisemap::strip_noisy(int strip)
{
	for (int stripno : *(this->noise_strips))
	{
		if (strip == stripno)
		{
			return true;
			break;
		}
	}
	return false;
}
