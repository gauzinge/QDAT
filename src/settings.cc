//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

#include "settings.h"

settings::settings(std::string file)
{
	this->filename = file;
}

settings::~settings()
{
	
}

void settings::read_process_settings()
{
	pugi::xml_document doc;
	doc.load_file(this->filename.c_str());
	if (!doc) std::cerr << "Could not load Setting file " << filename << std::endl;
	
	//processors
	this->calibrate = false;
	this->raw_data = false;
	this->pedenoise = false;
	this->noisemap = false;
	this->calibrate_data = false;
	this->clustering = false;
	
	if (atoi(doc.child("processors").child_value("calibration")) == 1) this->calibrate = true;
	if (atoi(doc.child("processors").child_value("raw_data")) == 1) this->raw_data = true;
	if (atoi(doc.child("processors").child_value("pedenoise")) == 1) this->pedenoise = true;
	if (atoi(doc.child("processors").child_value("noisemap")) == 1) this->noisemap = true;
	if (atoi(doc.child("processors").child_value("data")) == 1) this->calibrate_data = true;
	if (atoi(doc.child("processors").child_value("clustering")) == 1) this->clustering = true;
	
	//config
	configfile = doc.child("config").child_value("filename");
	run = atoi(doc.child("config").child_value("run"));
	// runstring = doc.child("config").child_value("run");
	char buffer[5];
	sprintf(buffer,"%03d", run);
	runstring = buffer;
	cal_run = atoi(doc.child("config").child_value("calibration_run"));
	// cal_runstring = doc.child("config").child_value("calibration_run");
	char calbuffer[5];
	sprintf(calbuffer,"%03d", cal_run);
	cal_runstring = calbuffer;

	datapath = doc.child("config").child_value("datapath");
	cal_datapath = doc.child("config").child_value("calibration_datapath");
	result_datapath = doc.child("config").child_value("result_datapath");
	
	//calibration
	nominal_signal = atoi(doc.child("calibration").child_value("nominal_signal"));
	
	//raw_data
	max_events = atoi(doc.child("raw_data").child_value("n_events"));
	
	//pedenoise
	std::string cm_string = doc.child("pedenoise").child_value("common_mode");
	cm = true;
	if (cm_string == "false") cm = false;
	cm_group = atoi(doc.child("pedenoise").child_value("cm_group"));
	n_discarded_strips = atoi(doc.child("pedenoise").child_value("n_discard_strips"));
	hitrejectioncut = atof(doc.child("pedenoise").child_value("hitrejectcut"));
	algorithm = doc.child("pedenoise").child_value("algorithm");
	cm_loops = atoi(doc.child("pedenoise").child_value("cm_loops"));
	
	//noisemap
	reject_cut = atof(doc.child("noisemap").child_value("reject_cut"));
	stripgroup = atoi(doc.child("noisemap").child_value("stripgroup"));
	
	//data
	htr_events = atoi(doc.child("data").child_value("HTR_events"));
	cluwindow = atoi(doc.child("data").child_value("fixed_clusterwindow"));
	seed = atof(doc.child("data").child_value("seed_cut"));
	
	//cluster
	clu_seed = atof(doc.child("cluster").child_value("clu_seed"));
	clu_neighbor = atof(doc.child("cluster").child_value("clu_neighbor"));
	clu_total = atof(doc.child("cluster").child_value("clu_total"));
	
	std::cout << "******************************************************" << std::endl;
	std::cout << "[SETTINGS] Reading Settings for ./process from " << filename << std::endl;
	std::cout << "[CONFIG]" << std::endl;
	std::cout << "Filename " << configfile << std::endl;
	std::cout << "Run " << run << std::endl
		<< "Calibration Run " << cal_run << std::endl
			<< "Data Path " << datapath << std::endl
				<< "Calibration Data Path " << cal_datapath << std::endl
					<< "Result Data Path " << result_datapath << std::endl << std::endl;
	
	std::cout << "[CALIBRATION]" << std::endl
		<< "Nominal Electron Signal " << nominal_signal << std::endl << std::endl;
	
	std::cout << "[RAW_DATA]" << std::endl
		<< "Max Events (-1 = all) " << max_events << std::endl << std::endl;
	
	std::cout << "[PEDENOISE] " << std::endl
		<< "Common Mode " << cm_string << std::endl
			<< "CM group " << cm_group << std::endl
				<< "N ignore Strips " << n_discarded_strips << std::endl
					<< "Hit Rejection Cut " << hitrejectioncut << std::endl
						<< "Algorithm " << algorithm << std::endl
							<< "CM Loops " << cm_loops << std::endl << std::endl;

	std::cout << "[NOISEMAP]" << std::endl
		<< "BadStrip Cut " << reject_cut << std::endl
			<< "Stripgroup " << stripgroup << std::endl << std::endl;
	
	std::cout << "[DATA]" << std::endl
		<< "# Events for Hit Time Reconstruction " << htr_events << std::endl
			<< "Seed neighbors for FF Cluster " << cluwindow << std::endl
				<< "Seed Threshold " << seed << std::endl << std::endl;
	
	std::cout << "[CLUSTER]" << std::endl
		<< "Seed Cut " << clu_seed << std::endl
			<< "Neighbor Cut " << clu_neighbor << std::endl
				<< "Total Cut " << clu_total << std::endl;
	
	std::cout << "******************************************************" << std::endl;
}


void settings::read_plot_settings()
{
	pugi::xml_document doc;
	doc.load_file(this->filename.c_str());
	if (!doc) std::cerr << "Could not load Setting file " << filename << std::endl;
	else
	{	
		//config
		configfile = doc.child("config").child_value("filename");
		run = atoi(doc.child("config").child_value("run"));
		// runstring = doc.child("config").child_value("run");
		char buffer[5];
		sprintf(buffer,"%03d", run);
		runstring = buffer;
		cal_run = atoi(doc.child("config").child_value("calibration_run"));
		// cal_runstring = doc.child("config").child_value("calibration_run");
		char calbuffer[5];
		sprintf(calbuffer,"%03d", cal_run);
		cal_runstring = calbuffer;

		datapath = doc.child("config").child_value("datapath");
		cal_datapath = doc.child("config").child_value("calibration_datapath");
		result_datapath = doc.child("config").child_value("result_datapath");
	
		//calibration
		nominal_signal = atoi(doc.child("calibration").child_value("nominal_signal"));
		std::cout << "[SETTINGS] Reading plotsettings!" << std::endl;
		pugi::xml_node nodeModules = doc.child("modules");
		
		std::cout << "******************************************************" << std::endl;
		std::cout << "[PROCESSORS]" << std::endl
			<<"Calibrate " << this->calibrate << std::endl
				<<"Raw Data " << this->raw_data << std::endl
					<<"PedeNoise " << this->pedenoise << std::endl
						<<"Noisemap " << this->noisemap << std::endl
							<<"Convert Data " << this->calibrate_data << std::endl
								<<"CLustering " << this->clustering << std::endl << std::endl; 
		
		std::cout << "******************************************************" << std::endl;
		std::cout << "[CONFIG]" << std::endl;
		std::cout << "Filename " << configfile << std::endl;
		std::cout << "Run " << run << std::endl
			<< "Calibration Run " << cal_run << std::endl
				<< "Data Path " << datapath << std::endl
					<< "Calibration Data Path " << cal_datapath << std::endl
						<< "Result Data Path " << result_datapath << std::endl << std::endl;
	
		std::cout << "[CALIBRATION]" << std::endl
			<< "Nominal Electron Signal " << nominal_signal << std::endl << std::endl;
	
		//PLOTS
		this->beamprofile = false;
		this->noiseprofile = false;
		this->clusterwidth =false;
		this->eta = false;
		this->signal = false;
		this->snr = false;
		
		if (atoi(doc.child("plots").child_value("beamprofile")) == 1) this->beamprofile = true;
		if (atoi(doc.child("plots").child_value("noiseprofile")) == 1) this->noiseprofile = true;
		if (atoi(doc.child("plots").child_value("clusterwidth")) == 1) this->clusterwidth = true;
		if (atoi(doc.child("plots").child_value("eta")) == 1) this->eta = true;
		if (atoi(doc.child("plots").child_value("signal")) == 1) this->signal = true;
		if (atoi(doc.child("plots").child_value("snr")) == 1) this->snr = true;
		
		std::cout << "******************************************************" << std::endl;
		std::cout << "[PLOTS]" << std::endl
			<<"Beamprofile " << this->beamprofile << std::endl
				<<"Noiseprofile " << this->noiseprofile << std::endl
					<<"Clusterwidth " << this->clusterwidth << std::endl
						<<"Eta " << this->eta << std::endl
							<<"Signal " << this->signal << std::endl
								<<"Signal-to-Noise " << this->snr << std::endl << std::endl;
	
		for (pugi::xml_node_iterator it = nodeModules.begin(); it != nodeModules.end(); it++)
		{
			std::cout << "******************************************************" << std::endl;
			std::cout << "Found Module in Settings: " << std::endl;
			std::cout << "Name " << it->child_value("name") << std::endl;
			std::cout << "ROI " << it->child_value("ROI_limits") << std::endl;
			std::cout << "HV " << it->child_value("HV") << std::endl;
			std::cout << "******************************************************" << std::endl;
			std::cout << std::endl;
			
			modulesettings mymodule;
			mymodule.HV = atoi(it->child_value("HV"));
			std::stringstream ss;
			ss << it->child_value("ROI_limits");
			ss >> mymodule.low_ROI >> mymodule.high_ROI;
			
			this->modulemap[it->child_value("name")] = mymodule;
		}
	}
}

bool settings::in_roi(int stripnumber, std::string name)
{
	std::map<std::string, modulesettings>::iterator it = this->modulemap.find(name);
	if (it == this->modulemap.end()) return false;
	else 
	{
		if (stripnumber < it->second.low_ROI || stripnumber > it->second.high_ROI) return false;
		else return true;
	}
}