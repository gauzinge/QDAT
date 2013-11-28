//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

#include "config.h"
#include <fstream>
#include <string>


config::config()
{
	
}

config::~config()
{
	this->resultfile->Write();
	// this->resultfile->Close();
	this->plotfile->Write();
	// this->plotfile->Close();
}


module* config::find_module(std::string name)
{
	for (unsigned int i = 0; i < this->modules.size(); i++)
	{
		// std::cout << "1	" << name.length() << std::endl;
// 		std::cout << "2 " << this->modules.at(i).name.length()<< std::endl;
		if (name == this->modules.at(i).name)
		{
			return &(this->modules.at(i));
			break;
		}
	}
	return NULL;
}

std::vector<double> config::read_configline (std::string rawline)
{
	std::vector<double> values;
	
	std::istringstream line(rawline);
	std::string trunc_line;
	char dummy;
	line >> dummy >> trunc_line;
	if (trunc_line.find(",x") != std::string::npos) trunc_line = trunc_line.substr(0,trunc_line.length()-2);
	std::stringstream aline(trunc_line);
	std::string field;
	while (std::getline(aline,field,','))
	{
		std::stringstream fs(field);
		double f = 0;
		fs >> f;
		values.push_back(f);
	}	
	return values;
}

void config::read_moduleline (std::string rawline/*, module& amodule*/)
{
	std::stringstream aline(rawline);
	std::string field;
	int counter = 0;
	int module = 0;
	std::string dummyname;
	
	while (std::getline(aline,field,','))
	{
		std::stringstream fs(field);
		if (counter == 0) fs >> module;
		this->modules.at(module).position = module;
		if (counter == 1) fs >> this->modules.at(module).crate;
		if (counter ==13) dummyname = field;
		counter++;
	}	
	dummyname.erase(std::remove_if(dummyname.begin(), dummyname.end(),isspace));
	dummyname.erase(std::remove(dummyname.begin(), dummyname.end(),'\r'),dummyname.end());
	this->modules.at(module).name = dummyname;
	// std::cout << "Module " << this->modules.at(module).name << std::endl;
}

void config::read_apvline (std::string rawline/*, apv& myapv*/)
{
	std::stringstream aline(rawline);
	std::string field;
	int counter = 0;
	int module = 0;
	int position = 0;
	while (std::getline(aline,field,','))
	{
		std::stringstream fs(field);
		if (counter == 0) fs >> module;
		if (counter == 1) fs >> position;
		this->modules.at(module).chips.at(position).module = module;
		this->modules.at(module).chips.at(position).position = position;
		if (counter == 2) fs >> this->modules.at(module).chips.at(position).i2c_adress;
		// if (counter == 8) fs >> this->modules.at(module).chips.at(position).fadc_data_offset;
		if (counter == 9) fs >> this->modules.at(module).chips.at(position).fadc_number;
		if (counter == 10) fs >> this->modules.at(module).chips.at(position).fadc_channel;
		if (counter == 11) fs >> this->modules.at(module).chips.at(position).fadc_clk_delay;
		counter++;
	}	
	// std::cout << "Chip " << this->modules.at(module).chips.at(position).position << " on module " << module << " with I2C " << this->modules.at(module).chips.at(position).i2c_adress << std::endl;
}

void config::read_apvline_2 (std::string rawline/*, apv& myapv*/)
{
	std::stringstream aline(rawline);
	std::string field;
	int counter = 0;
	int module = 0;
	int chip = 0;
	while (std::getline(aline,field,','))
	{
		std::stringstream fs(field);
		if (counter == 1) fs >> module; 
		if (counter == 2) fs >> chip;
		if (counter == 3) fs >> this->modules.at(module).chips.at(chip).first_strip;
		counter++;
	}	
	// std::cout << "First strip " <<this->modules.at(module).chips.at(chip).first_strip << std::endl;
}

void config::read_sensorline (std::string rawline)
{
	std::stringstream aline(rawline);
	std::string field;
	int counter = 0;
	int i = 0;
	while (std::getline(aline,field,','))
	{
		std::stringstream fs(field);
		if (counter == 0) fs >> i;
		this->modules.at(i).sensor_number = i;
		if (counter == 1) fs >> this->modules.at(i).sensor_strips;
		if (counter == 5) fs >> this->modules.at(i).first_strip;
		if (counter == 6) fs >> this->modules.at(i).last_strip;
		if (counter == 7) fs >> this->modules.at(i).pitch;
		if (counter ==10) fs >> this->modules.at(i).strip_length;
		counter++;
	}	
	// std::cout << this->modules.at(i).sensor_number << "  " << this->modules.at(i).sensor_strips << " " << this->modules.at(i).pitch << std::endl;
}

void config::read_globalsettings(std::string filename)
{
	std::ifstream configfile(filename.c_str());
	if (!configfile) std::cerr << "The specified configfile " << filename << " does not exist!" << std::endl;
	else if (configfile.is_open())
	{
		while (!configfile.eof())
		{
			std::string fileline;
			std::string keyword;
			std::string rawline;
			configfile >> keyword;
			std::getline(configfile,rawline);
			
			if (keyword.compare(0,1,"#") != 0 && keyword.compare(0,1,";") != 0) // line is not commented
			{
				if (keyword == "clk")
				{
					std::vector<double> values = this->read_configline(rawline);
					this->clk = values[0];
					if (values[1] == 0) this->frequency = 40;
				}
				if (keyword == "ads")
				{
					std::vector<double> values = this->read_configline(rawline);
					this->n_adc_samples = values[0];
					this->n_subevents = values[1];
				}
				if (keyword == "ini")
				{
					std::vector<double> values = this->read_configline(rawline);
					this->n_pedenoiseevents = 2 * values[0];
				}
				if (keyword == "hcs")
				{
					std::vector<double> values = this->read_configline(rawline);
					this->seed_cut = values[0];
					this->neighbor_cut = values[1];
				}
				if (keyword == "lvl")
				{
					std::vector<double> values = this->read_configline(rawline);
					this->cal_lvl = values[0];
				}
					
				
			}
			if (configfile.eof()) break;
		}
	}
	configfile.close();
}

void config::read_modulesettings(std::string filename)
{
	int module_counter=0;
	
	std::ifstream configfile(filename.c_str());
	if (!configfile) std::cerr << "The specified configfile " << filename << " does not exist!" << std::endl;
	else if (configfile.is_open())
	{
		while (!configfile.eof())
		{
			std::string line;
			std::getline(configfile,line);
			if (line.compare(0,1,"#") != 0 && line.compare(0,1,";") != 0 && line.substr(0,5) == "[mod]")
			{
				std::string modline;
				std::string apvline;
				while (std::getline(configfile,modline))
				{
					if (modline.substr(0,1) != "#" && modline.substr(0,3) == "mod")
					{
						module amodule;
						this->modules.push_back(amodule);
						int apv_counter = 0;
						modline = modline.substr(5, modline.length()-5);
						//read properties of module
						this->read_moduleline(modline);
						while (std::getline(configfile,apvline))
						{
							
							if (apvline.substr(0,3) == "apv")
							{
								apvline = apvline.substr(5, apvline.length()-5);
								apv myapv;
								this->modules.at(module_counter).chips.push_back(myapv);																
								this->read_apvline(apvline);
								apv_counter++;
							} //apv
							else break;
						} //apv
						this->modules.at(module_counter).n_apv = this->modules.at(module_counter).chips.size();
						module_counter++;
					} //module
				} //module
			}
			if (configfile.eof()) break;
		}
	}
	this->n_modules = module_counter;
	configfile.close();

	//2nd time for sensor data 
	configfile.open(filename.c_str());
	if (!configfile) std::cerr << "The specified configfile " << filename << " does not exist!" << std::endl;
	else if (configfile.is_open())
	{
		while (!configfile.eof())
		{
			std::string line;
			std::getline(configfile,line);
			if (line.compare(0,1,"#") != 0 && line.compare(0,1,";") != 0 && line.substr(0,5) == "[sen]"){
				std::string senline;
				std::string apvline;
				while (std::getline(configfile,senline))
				{
					if (senline.substr(0,1) != "#" && senline.substr(0,3) == "sen")
					{
						senline = senline.substr(5, senline.length()-5);
						this->read_sensorline (senline);
						while (std::getline(configfile,apvline))
						{
							if (apvline.substr(0,3) == "apv")
							{
								apvline = apvline.substr(5, apvline.length()-5);
								this->read_apvline_2(apvline);
							} //apv
							else break;
						} //apv
					}
				}	
			}
			if (configfile.eof()) break;
		}
	}
	configfile.close();
}

void config::read_config(settings* mysettings)
{
	std::string filename = mysettings->configfile;
	this->run = mysettings->run;
	this->calrun = mysettings->cal_run;
	this->runstring = mysettings->runstring;
	this->calstring = mysettings->cal_runstring;
	
	this->path = mysettings->datapath;
	this->calpath = mysettings->cal_datapath;
	this->resultpath = mysettings->result_datapath;
		
	this->read_globalsettings(filename);
	this->read_modulesettings(filename);
	
	//init resultfile
	std::string resultfilename = mysettings->result_datapath + "Results_run" + this->runstring + ".root";
	this->resultfile = new TFile(resultfilename.c_str(),"UPDATE"); //change this at some point
	std::cout << "Opening Resultfile " << resultfilename << std::endl;
	
	//init plotfile
	std::string plotfilename = mysettings->result_datapath + "Plots_run" + this->runstring + ".root";
	this->plotfile = new TFile(plotfilename.c_str(),"UPDATE"); //change this at some point
	std::cout << "Opening Plotfile " << plotfilename << std::endl;
	
	//compute number of apvs per fadc
	int n_fadc = 0;
	int fadc_apvs = 0;
	this->max_apv = 0;
	int first_strip = 0;
	for (int i=0; i<this->n_modules;i++)
	{
		this->modules.at(i).global_first_strip = first_strip;
		for (int j = 0; j < this->modules.at(i).n_apv; j++)
		{
			if (this->modules.at(i).chips.at(j).fadc_number == n_fadc) 
			{
				this->modules.at(i).chips.at(j).fadc_data_offset = fadc_apvs * (this->n_adc_samples+1)+1;
			}
			else 
			{
				n_fadc++;
				fadc_apvs = 0;
				this->modules.at(i).chips.at(j).fadc_data_offset = fadc_apvs * (this->n_adc_samples+1)+1;
			}
			this->modules.at(i).chips.at(j).global_apvnumber = this->max_apv;
			this->max_apv++;
			fadc_apvs++;
		}
		first_strip += this->modules.at(i).last_strip + 1;
	}
	this->max_fadc = n_fadc+1;
	
	std::cout << "**********************************" << std::endl
		<< "CONFIG" << std::endl
			<< "**********************************" << std::endl;
	
	std::cout  << "Read the following Settings from Configfile " << filename << " for Run " << this->run << " : " << std::endl
		<< "General DAQ Settings: " << std::endl
			<< "N_ADC_Samples " << this->n_adc_samples << std::endl 
				<< "N_Subevents " << this->n_subevents << std::endl
					<< "N_PedenoiseEvents " << this->n_pedenoiseevents << std::endl
						<< "N_APV " << this->max_apv << std::endl
							<< "N_FADC " << this->max_fadc << std::endl
								<< "Seed_Cut " << this->seed_cut << std::endl
									<< "Neighbor_Cut " << this->neighbor_cut << std::endl
										<< "Calibration Level " << this->cal_lvl << std::endl 
											<< "**********************************" << std::endl
											<< std::endl;

	std::cout << "Found " << this->n_modules << " Modules in the file with settings: " << std::endl;
	for (int i=0; i<this->n_modules;i++)
	{
		std::cout << "Name: " << this->modules.at(i).name << std::endl;
		std::cout << "Position: " << this->modules.at(i).position << " Crate: " << this->modules.at(i).crate << " # APVs: " << this->modules.at(i).n_apv << " # Sensor: " << this->modules.at(i).sensor_number << " # Strips: " << this->modules.at(i).sensor_strips << " 1st Strip: " << this->modules.at(i).first_strip << " last Strip: " << this->modules.at(i).last_strip << " Global 1st Strip: " << this->modules.at(i).global_first_strip << " Pitch: " << this->modules.at(i).pitch << " Strip Length [mu] " <<this->modules.at(i).strip_length << std::endl;
		
		std::cout << "APV Settings: " << std::endl;
		
		for (int j = 0; j < this->modules.at(i).n_apv; j++)
		{
			std::cout << "Module #: " << this->modules.at(i).chips.at(j).module << " APV #: " << this->modules.at(i).chips.at(j).position << " First Strip: " << this->modules.at(i).chips.at(j).first_strip << " I2C Adress: " << this->modules.at(i).chips.at(j).i2c_adress << " fadc_number: " << this->modules.at(i).chips.at(j).fadc_number << " fadc_channel: " << this->modules.at(i).chips.at(j).fadc_channel << " Global APV number: " << this->modules.at(i).chips.at(j).global_apvnumber << " fadc_data_offset " << this->modules.at(i).chips.at(j).fadc_data_offset << std::endl;
		}
		std::cout << "**********************************" << std::endl << std::endl;
	}
}

void config::read_config_silent(settings* mysettings)
{
	std::string filename = mysettings->configfile;
	this->run = mysettings->run;
	this->calrun = mysettings->cal_run;
	this->runstring = mysettings->runstring;
	this->calstring = mysettings->cal_runstring;
	
	this->path = mysettings->datapath;
	this->calpath = mysettings->cal_datapath;
	this->resultpath = mysettings->result_datapath;
	 
		
	this->read_globalsettings(filename);
	this->read_modulesettings(filename);
	
	//init resultfile
	std::string resultfilename = mysettings->result_datapath + "Results_run" + this->runstring + ".root";
	this->resultfile = new TFile(resultfilename.c_str(),"UPDATE"); //change this at some point
	std::cout << "Initializing new Resultfile " << resultfilename << std::endl;
	
	//init plotfile
	std::string plotfilename = mysettings->result_datapath + "Plots_run" + this->runstring + ".root";
	this->plotfile = new TFile(plotfilename.c_str(),"UPDATE"); //change this at some point
	std::cout << "Opening Plotfile " << plotfilename << std::endl;
	
	//compute number of apvs per fadc
	int n_fadc = 0;
	int fadc_apvs = 0;
	this->max_apv = 0;
	int first_strip = 0;
	for (int i=0; i<this->n_modules;i++)
	{
		this->modules.at(i).global_first_strip = first_strip;
		for (int j = 0; j < this->modules.at(i).n_apv; j++)
		{
			if (this->modules.at(i).chips.at(j).fadc_number == n_fadc) 
			{
				this->modules.at(i).chips.at(j).fadc_data_offset = fadc_apvs * (this->n_adc_samples+1)+1;
			}
			else 
			{
				n_fadc++;
				fadc_apvs = 0;
				this->modules.at(i).chips.at(j).fadc_data_offset = fadc_apvs * (this->n_adc_samples+1)+1;
			}
			this->modules.at(i).chips.at(j).global_apvnumber = this->max_apv;
			this->max_apv++;
			fadc_apvs++;
		}
		first_strip += this->modules.at(i).last_strip + 1;
	}
	this->max_fadc = n_fadc+1;
}


// void config::get_maxevents (std::string textfilename)
// {
// 	int max_eventnumber = 0;
// 	std::ifstream textfile(textfilename.c_str());
// 	if (!textfile) std::cerr << "The specified textfile " << textfilename << " does not exist!" << std::endl;
// 	else if (textfile.is_open())
// 	{
// 		int linecounter = 0;
// 		while (!textfile.eof())
// 		{
// 			std::string line;
// 			std::getline(textfile, line);
// 			linecounter++;
// 			std::stringstream aline(line);
// 			std::string field;
// 			int counter = 0;
// 			if (linecounter > 23)
// 			{
// 				while (std::getline(aline,field,' '))
// 				{
// 					std::stringstream fs(field);
// 					if (counter == 4) 
// 					{
// 						int event_number = 0;
// 						if (field.compare(0,5,"event=") != 0)
// 						{
// 							field =  field.substr(6, field.length()-6);
// 							std::stringstream number(field);
// 							number >> event_number;
// 							if (event_number >  max_eventnumber) max_eventnumber = event_number;
// 						}
// 					}
// 					counter++;
// 				}	
// 			}
// 			if (textfile.eof()) break;
// 		}
// 	}
// 	this->maxevents = max_eventnumber;
// }ber;
// }