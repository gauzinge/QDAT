//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#include "calibration.h"

calibration::calibration(config* theconfig)
{
	this->aconfig = theconfig;
	for (unsigned int mod = 0; mod < this->aconfig->modules.size(); mod++)
	{
		// std::cout << this->aconfig->modules.size() << " " << this->aconfig->modules.at(mod).chips.size() << std::endl;
		for (unsigned int apv = 0; apv < this->aconfig->modules.at(mod).chips.size(); apv++)
		{
			TMultiGraph* dummy =  new TMultiGraph();
			this->apv_curves.push_back(dummy);
		}
	}
}

calibration::~calibration()
{
	
}

void calibration::get_calfactors(int nominal_signal)
{
	this->nominal = nominal_signal;
	
	std::string filename = this->aconfig->calpath + "cal" + this->aconfig->calstring + ".cal";
	std::ifstream calfile;
	calfile.open(filename.c_str());
	
	if (!calfile) std::cerr << "Could not open Calibration File!" << std::endl;
	else
	{
		std::cout << "[CALIBRATION] Reading Calibration File!" << std::endl;
		
		//initialize a std::vector<TGraph*> channels;
		std::vector<TGraph*> channels;
		
		for (int i = 0; i < this->aconfig->max_apv * 128; i++)
		{
			TGraph* dummy = new TGraph();
			channels.push_back(dummy);
		}

		while (!calfile.eof())
		{
			std::string line;
			std::getline(calfile,line);
			if (line.find("apv") == std::string::npos && line.find("tim") == std::string::npos)
			{
				std::stringstream helpstream (line);
				int apvnr;
				int channel;
				double time;
				double adc;
				double dummy;
				
				helpstream >> apvnr >> channel >> time >> adc >> dummy >> dummy;
				int global_ch = apvnr * 128 + channel;
				// std::cout << apvnr << " " << channel << " " << global_ch << " " << time << " " << adc << std::endl;
				 
				channels.at(global_ch)->SetPoint(channels.at(global_ch)->GetN(),time,adc);
			}
			if (calfile.eof()) break;
		}
		std::cout << "[CALIBRATION] Curves created, now finding the Calibration Factor for each Channel!" << std::endl;
		
		for (unsigned int chan = 0; chan < channels.size(); chan++)
		{
			int apvnr = static_cast<int>(chan/128);
			
			int n = channels.at(chan)->GetN();
			double *y = channels.at(chan)->GetY();
			int max = TMath::LocMax(n,y);
			double maxval = y[max];
			double factor = nominal_signal / maxval;
			
			this->help_calibfactor.push_back(factor);
			this->apv_curves.at(apvnr)->Add(channels.at(chan),"l");
			
			if (chan % 128 == 0) std::cout << "Finished " << apvnr << " APVs!" << std::endl;
		}
		
		//save to file
		for (unsigned int apvnr = 0; apvnr < this->apv_curves.size(); apvnr++)
		{
			std::stringstream st;
			std::stringstream title;
			st << "Calibration_Curves_APV_" << apvnr << ";Time;ADC";
			title << "CalCurves_APV_" << apvnr;
			this->apv_curves.at(apvnr)->SetTitle(st.str().c_str());
			this->aconfig->resultfile->WriteObject(this->apv_curves.at(apvnr),title.str().c_str());
		}
		
	}
	std::string rootfilename = this->aconfig->calpath + "cal" + this->aconfig->calstring + ".root";
	TFile* caldatafile = TFile::Open(TFile::AsyncOpen(rootfilename.c_str(),"RECREATE"));
	if (!caldatafile) std::cerr << "Could not open File for Calibration factors!" << std::endl;
	else
	{
		TTree* caldatatree = new TTree("caldata", "Calibration Factors for each strip");
		caldatatree->Branch("nominal_signal", &nominal_signal);
		caldatatree->Branch("calfactors", &this->help_calibfactor);
		
		caldatatree->Fill();
		caldatafile->Write();
		caldatafile->Close();
		std::cout << "[CALIBRATION] Successfully created Calibration Factor Collection!" << std::endl;
	}
}

void calibration::load_calibration()
{
	this->calfactor = NULL;
	std::cout << "[CALIBRATION] Loading calibration factors for all channels into memory!" << std::endl;
	
	std::string rootfilename = this->aconfig->calpath + "cal" + this->aconfig->calstring + ".root";
	TFile* file = TFile::Open(TFile::AsyncOpen(rootfilename.c_str(),"READ"));
	if (!file) std::cerr << "Could not open Calibration factor file!" << std::endl;
	else
	{
		TTree* tree = (TTree*) file->Get("caldata");
		TBranch* nominalbranch = 0;
		TBranch* calbranch = 0;
		
		tree->SetBranchAddress("nominal_signal",&this->nominal,&nominalbranch);
		tree->SetBranchAddress("calfactors",&this->calfactor,&calbranch);
		tree->GetEntry(0);
		std::cout << "[CALIBRATION] Successfully loaded calibration factors!" << std::endl;
	}
}