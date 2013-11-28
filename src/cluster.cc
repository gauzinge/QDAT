//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

#include "cluster.h"

cluster::cluster(config* theconfig) : apedenoise(theconfig) , anoisemap(theconfig) ,  adata(theconfig)
{
	this->aconfig = theconfig;
	apedenoise.read_pedenoise();
	
	anoisemap.load_noisemap();
	adata.load_tree();
	
	// this->clusterfile = NULL;
// 	this->clustertree = NULL;
}

cluster::~cluster()
{
	
}

void cluster::find_clusters(double seed, double neighbor, double total)
{
	std::cout << "[CLUSTERING] Running Clustersearch with " << seed << " as seed cut, " << neighbor << " as neighbor and " << total << " as total SNR cut!" << std::endl;
	
	//first the tree
	std::string filename = this->aconfig->resultpath + "run" + this->aconfig->runstring + "_cluster.root";
	TFile* aclusterfile = TFile::Open(TFile::AsyncOpen(filename.c_str(),"RECREATE"));
	if (!aclusterfile) std::cerr << "Could not create File for Clusters!" << std::endl;
	else
	{
		TTree* aclustertree = new TTree("clusters","Clusters");
		if (!aclustertree) std::cerr << "Could not create TTree for Cluster Data!" << std::endl;
		else
		{
			aclustertree->Branch("modulename", &this->modulename);
			aclustertree->Branch("event", &this->event);
			aclustertree->Branch("first_strip", &this->first_strip);
			aclustertree->Branch("seed_strip", &this->seed_strip);
			aclustertree->Branch("cog", &this->cog);
			aclustertree->Branch("signal", &this->signal);
			aclustertree->Branch("noise", &this->noise);
			aclustertree->Branch("seed_noise", &this->seed_noise);
			aclustertree->Branch("eta", &this->eta);
			aclustertree->Branch("strip_signals", &this->help_str_signals);
	
			int clustercounter = 0;
			for (unsigned int event = 0; event < this->adata.get_n_events(); event++) // looping over events
			{
				this->adata.get_event(event);
		
				for (unsigned int mod = 0; mod < this->aconfig->modules.size(); mod++) // looping over modules
				{
					module* themodule = &(this->aconfig->modules.at(mod));	
							
					//now search for clusters
					int strip = themodule->first_strip;
					int global_strip = themodule->global_first_strip;
			
					while (strip < themodule->last_strip)
					{	
						//look for seed
						if (this->adata.adc->at(global_strip) / this->apedenoise.noise->at(global_strip) > seed && !this->anoisemap.strip_noisy(global_strip)) //seed found and not a noiy strip
						{							
							//found a seed, declare help list
							std::list<double> help;
							this->help_str_signals.clear();
					
							this->modulename = this->aconfig->modules.at(mod).name;
							this->event = this->adata.event;
							this->first_strip = strip;
							this->signal = this->adata.adc->at(global_strip);
					
							help.push_back(this->adata.adc->at(global_strip));
							int width = 1;
							double tmp_sq_noise = pow(this->apedenoise.noise->at(global_strip),2);
							double weighted_sum = this->adata.adc->at(global_strip) * static_cast<double>(strip);
										
							int up = 1;
							int down = 1;
					
							//searchfor neighbors.
							while (strip - down >= themodule->first_strip && this->adata.adc->at(global_strip - down) / this->apedenoise.noise->at(global_strip - down) > neighbor)
							{								
									width++;
									this->signal += this->adata.adc->at(global_strip - down);
									help.push_front(this->adata.adc->at(global_strip - down));
									tmp_sq_noise += pow(this->apedenoise.noise->at(global_strip - down),2);
									weighted_sum += this->adata.adc->at(global_strip - down) * static_cast<double>((strip - down));
									//if the cluster is larger than one strip, this is the new first strip
									this->first_strip = strip - down;
									down++;
							}
							while (strip + up <= themodule->last_strip && this->adata.adc->at(global_strip + up) / this->apedenoise.noise->at(global_strip + up) > neighbor)
							{
									width++;
									this->signal += this->adata.adc->at(global_strip + up);
									help.push_back(this->adata.adc->at(global_strip + up));
									tmp_sq_noise += pow(this->apedenoise.noise->at(global_strip + up),2);
									weighted_sum += this->adata.adc->at(global_strip + up) * static_cast<double>((strip + up));									
									up++;
									
							}
					
							//here I finally should have a nice cluster, now calculate clusternoise, cog, eta and seed strip and check for noise strips
							this->noise = sqrt(tmp_sq_noise);
							this->cog = weighted_sum / this->signal;
					
							double maxsignal = 0;
							int counter = 0;						
														
							bool noisy = false;
							for (std::list<double>::iterator element = help.begin(); element != help.end(); element++)
							{
								if (*element > maxsignal) this->seed_strip = counter;
								this->help_str_signals.push_back(*element);
								noisy = this->anoisemap.strip_noisy(global_strip);								
								if (noisy) break;
								counter++;
							}
							int local_seedstrip = this->first_strip + this->seed_strip;
							int global_seedstrip = themodule->global_first_strip + local_seedstrip;
							this->seed_noise = this->apedenoise.noise->at(global_seedstrip);
							
							// if (this->help_str_signals.size() > 1) std::cout << "Cluster " << global_strip << " signal " << signal << " first strip " << first_strip << " global first strip " << themodule->global_first_strip + first_strip <<  " noise " << noise << std::endl;
							
							//search highest neighbor here and calculate eta!
							if (this->help_str_signals.size() == 2)
							{
								this->eta = this->help_str_signals.at(0)/(this->help_str_signals.at(0) + this->help_str_signals.at(1));
							}
							else if (this->help_str_signals.size() > 2)
							{
								double secondmax = 0;
								int index = 0;
								//search for second max strip and index
								for (unsigned int i = 0; i < this->help_str_signals.size(); i++)
								{
									if (this->help_str_signals.at(i) > secondmax && this->help_str_signals.at(i) < maxsignal)
									{
										secondmax = this->help_str_signals.at(i);
										index = i;
									}
								}
								if (index < this->seed_strip) this->eta = secondmax / (secondmax + maxsignal);
								else this->eta = maxsignal / (secondmax + maxsignal);
							}
							else if (this->help_str_signals.size() == 1)
							{
								//take module borders into account
								if (strip == themodule->first_strip) this->eta = this->signal/(this->adata.adc->at(global_strip + 1) + this->signal); //right
								else if (strip == themodule->last_strip) this->eta = this->adata.adc->at(global_strip - 1)/(this->adata.adc->at(global_strip - 1) + this->signal); //left
								else if (this->adata.adc->at(global_strip - 1) > this->adata.adc->at(global_strip + 1)) this->eta = this->adata.adc->at(global_strip - 1)/(this->adata.adc->at(global_strip - 1) + this->signal); //left
								else if (this->adata.adc->at(global_strip - 1) < this->adata.adc->at(global_strip + 1)) this->eta = this->signal/(this->adata.adc->at(global_strip + 1) + this->signal); //right
							}
					
							//now every property of the cluster is set, check for total cut and noisy strips
							if (this->signal / this->noise > total && !noisy)  
							{
								aclustertree->Fill();
								// std::cout << "Width " << this->help_str_signals.size() << " Signal " << this->signal << " Noise sum " << tmp_sq_noise << " Noise " << this->noise << std::endl;
								if (clustercounter % 10000 == 0) 
								{
									aclusterfile->Flush();
									std::cout << "Processed " << clustercounter << " Clusters!" << std::endl;
								}
								clustercounter++;
							}					
							strip += up;
							global_strip += up;
						}
						else
						{
							strip++;
							global_strip++;
						}
					} //end of while loop
				} //end of module loop
			} //end of event loop
			std::cout << "[CLUSTERING] Found " << clustercounter << " Clusters in Data!" << std::endl;
			aclusterfile->Write();
			aclusterfile->Close();
		}
	}
	std::cout << "[CLUSTERING] Successfully finished Clustering!" << std::endl;
}

void cluster::read_file()
{
	this->help = NULL;
	this->str_signals = NULL;
	std::string filename = this->aconfig->resultpath + "run" + this->aconfig->runstring + "_cluster.root";
	this->clusterfile = TFile::Open(TFile::AsyncOpen(filename.c_str(), "READ"));
	if (!this->clusterfile) std::cerr << "Could not open Cluster File!" << std::endl;
	else
	{
		this->clustertree = (TTree*) clusterfile->Get("clusters");
		if (!this->clustertree) std::cerr << "Could not decode the Cluster Tree!" << std::endl;
		else
		{
			TBranch* namebranch = 0;
			TBranch* eventbranch = 0;
			TBranch* firstbranch = 0;
			TBranch* seedbranch = 0;
			TBranch* cogbranch = 0;
			TBranch* signalbranch = 0;
			TBranch* noisebranch = 0;
			TBranch* seednoisebranch = 0;
			TBranch* etabranch = 0;
			TBranch* str_signalbranch = 0;
			
			this->clustertree->SetBranchAddress("modulename", &this->help, &namebranch);
			this->clustertree->SetBranchAddress("event", &this->event, &eventbranch);
			this->clustertree->SetBranchAddress("first_strip", &this->first_strip, &firstbranch);
			this->clustertree->SetBranchAddress("seed_strip", &this->seed_strip, &seedbranch);
			this->clustertree->SetBranchAddress("cog", &this->cog, &cogbranch);
			this->clustertree->SetBranchAddress("signal", &this->signal, &signalbranch);
			this->clustertree->SetBranchAddress("noise", &this->noise, &noisebranch);
			this->clustertree->SetBranchAddress("seed_noise", &this->seed_noise, &seednoisebranch);
			this->clustertree->SetBranchAddress("eta", &this->eta, &etabranch);
			this->clustertree->SetBranchAddress("strip_signals", &this->str_signals, &str_signalbranch);
		}
	}
}

int cluster::get_n_entries()
{
	if (this->clustertree == NULL)
	{
		std::cerr << "No Cluster Data available!" << std::endl;
		return 0;
	}
	return this->clustertree->GetEntries();
}

void cluster::get_entry(int i_entry)
{
	if (this->clustertree != NULL)
	{
		this->clustertree->GetEntry(i_entry);
		this->modulename = *this->help;
	}
}