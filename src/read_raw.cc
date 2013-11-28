//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

#include <iostream>
#include "definitions.h"
#include "read_raw.h"

raw_data::raw_data(config* theconfig)
{
	this->aconfig = *theconfig;
	this->init_channel_lookup();
	this->pedenoise_eventdata_file = NULL;
	this->eventdata_file = NULL;					 // lookup table to un-tangle APVMUX from Dataframe
}

raw_data::~raw_data()
{
	
}
	
void raw_data::init_trees()
{
	this->pedefilename = this->aconfig.resultpath + "run" + this->aconfig.runstring + "_pedenoise_data.root";
	this->datafilename = this->aconfig.resultpath + "run" + this->aconfig.runstring + "_adcdata.root";
	//Pedenoise
	
	this->pedenoise_eventdata_file = TFile::Open(TFile::AsyncOpen(this->pedefilename.c_str(),"RECREATE"));
	if (! this->pedenoise_eventdata_file) std::cerr << "Can not open Pedenoise Event Data File " << pedenoise_eventdata_file << std::endl;
	else 
	{
		this->pedenoise_data = new TTree("pedenoisedata_tree","Tree with event data for the Pedestal and Noise Events");
		if (! this->pedenoise_data) std::cerr << "Can not open Pedenoise Event Data Tree!" << std::endl;
		else {
			this->pedenoise_data->Branch("event_number", &this->myraweventdata.event);
			this->pedenoise_data->Branch("n_apv", &this->aconfig.max_apv);
			this->pedenoise_data->Branch("adc_vector", &this->myraweventdata.subevent0_data);
		}
	}
	
	//Data
	this->eventdata_file = TFile::Open(TFile::AsyncOpen(this->datafilename.c_str(),"RECREATE"));
	if (! this->eventdata_file) std::cerr << "Can not open Event Data File " << eventdata_file << std::endl;
	else 
	{
		this->datatree = new TTree("data_tree","Tree with event data!");
		if (! this->datatree) std::cerr << "Can not open Event Data Tree!" << std::endl;
		else {
			this->datatree->Branch("event_number", &this->myraweventdata.event);
			this->datatree->Branch("n_subevents", &this->myraweventdata.n_subevents);
			this->datatree->Branch("n_apv", &this->aconfig.max_apv);
			 
			this->datatree->Branch("adc_vector_0", &this->myraweventdata.subevent0_data); // think about this!!
			this->datatree->Branch("adc_vector_1", &this->myraweventdata.subevent1_data); // think about this!!
			this->datatree->Branch("adc_vector_2", &this->myraweventdata.subevent2_data); // think about this!!
			this->datatree->Branch("adc_vector_3", &this->myraweventdata.subevent3_data); // think about this!!
			this->datatree->Branch("adc_vector_4", &this->myraweventdata.subevent4_data); // think about this!!
			this->datatree->Branch("adc_vector_5", &this->myraweventdata.subevent5_data); // think about this!!
		}
	}
}

void raw_data::write_trees()
{
	this->pedenoise_eventdata_file->Write();
	this->pedenoise_eventdata_file->Close();
	std::cout << "Pedestal and Noise Collection in " << this->pedefilename << " successfully created!" << std::endl;
	this->eventdata_file->Write();
	this->eventdata_file->Close();
	std::cout << "Data Collection in " << this->datafilename << " successfully created!" << std::endl;
}

void raw_data::read_raw(int read_n_events)
{
	std::string filename = this->aconfig.path + "run" + this->aconfig.runstring + ".dat";
	//counters
	int filecounter = 1;
	// int evt_cnt = 0;
	//pedefile
	this->init_trees();
	
	std::ifstream datafile;
	datafile.open(filename.c_str(), std::ios::binary);

	if (!datafile) std::cerr << "The Datafile " << filename << " does not exist!" << std::endl;
	else
	{
		std::cout << "Reading Datafile " << filename << std::endl;
		while (!datafile.eof())
		{			
			datafile.read((char*) &this->save_event_header, sizeof( this->save_event_header));
				
			for (int i = 0; i < this->save_event_header.number_fadcs; i++)
			{								
				datafile.read((char*) &this->fadc_rawdata[i][0], sizeof(this->fadc_rawdata[0][0])*this->save_event_header.fadcdatalength[i]);
				if (FADC_CHK_MODE(this->save_event_header.fadcmode, FADC_MODE_TRP_2K))
				{					
					this->fill_fifo1_data();	
					this->process_raw_adc(save_event_header.eventcnt); 
				} 
				else if (FADC_CHK_MODE(this->save_event_header.fadcmode, FADC_MODE_HIT))
				{
					std::cerr << "FADC_MODE not TRANSPARENT, QUITING!" << std::endl;
				} 
				else
				{
					std::cerr << "FADC_MODE not TRANSPARENT, QUITING!" << std::endl;
				}
				// evt_cnt = this->save_event_header.eventcnt;
			}
			//fill pedenoise
			if (static_cast<int>(this->save_event_header.eventcnt) < this->aconfig.n_pedenoiseevents)
			{
				if (save_event_header.eventcnt%100 == 0) 
					std::cout << save_event_header.eventcnt << " PedeNoise Events converted!" << std::endl;
				this->pedenoise_data->Fill();
			}
			if (static_cast<int>(this->save_event_header.eventcnt) == this->aconfig.n_pedenoiseevents-1) 
			{
				this->pedenoise_eventdata_file->Flush();
				std::cout << "Flushing PedeNoise Data File!" << std::endl;
			}
			//fill data
			else if (static_cast<int>(this->save_event_header.eventcnt) >= this->aconfig.n_pedenoiseevents-1)
			{				
				this->datatree->Fill();
				if ((this->save_event_header.eventcnt - this->aconfig.n_pedenoiseevents)%1000 == 0) 
				{
					std::cout << this->save_event_header.eventcnt - this->aconfig.n_pedenoiseevents << " Data Events (Event Number " << this->save_event_header.eventcnt << ") with " <<  static_cast<int>(this->save_event_header.subevents) << " Subevents converted, Flushing Datafile!" << std::endl;
					this->eventdata_file->Flush();
				}
			}
			if (static_cast<int>(this->save_event_header.eventcnt) - this->aconfig.n_pedenoiseevents > read_n_events && read_n_events != -1) break;
			if (datafile.eof())
			{	
				// break;
				datafile.close();
				std::cout << "Done reading Datafile " << filename << ". Switching to next file ";
				//increment last digit by 1
				char buffer[5];
				sprintf(buffer,".%03d", filecounter);
				if (filecounter > 1) filename = filename.substr(0,filename.length()-4);
				filename = filename+buffer;
				std::cout << filename << std::endl;
				datafile.open(filename.c_str(), std::ios::binary);
				filecounter++;
				if (!datafile) 
				{
					std::cout << "Could not open Datafile " << filename << "!" << std::endl;
					datafile.close();
					break;
				}
			}
		}
		datafile.close();
		this->write_trees();
	}
}


void raw_data::fill_fifo1_data ()
{
	// clear data buffer adc_rawdata
	memset (this->adc_rawdata, 0, sizeof(this->adc_rawdata));

	int apvnumber = 0;
	for (unsigned int mod = 0; mod < this->aconfig.modules.size(); mod++)
	{
		for (unsigned int i = 0; i < this->aconfig.modules.at(mod).chips.size(); i++)
		{
			for (int j=0; j<this->aconfig.n_adc_samples; j++)
			{				
				this->adc_rawdata[apvnumber][j] = (this->fadc_rawdata[this->aconfig.modules.at(mod).chips.at(i).fadc_number][this->aconfig.modules.at(mod).chips.at(i).fadc_data_offset+j]>>23) & 0x1ff;
			}
			apvnumber++;
		}
	}
}

void raw_data::init_channel_lookup ()
{
	this->channel_lookup[0]=0;
	this->channel_lookup[1]=32;
	this->channel_lookup[2]=64;
	this->channel_lookup[3]=96;
	this->channel_lookup[4]=8;
	this->channel_lookup[5]=40;
	this->channel_lookup[6]=72;
	this->channel_lookup[7]=104;
	this->channel_lookup[8]=16;
	this->channel_lookup[9]=48;
	this->channel_lookup[10]=80;
	this->channel_lookup[11]=112;
	this->channel_lookup[12]=24;
	this->channel_lookup[13]=56;
	this->channel_lookup[14]=88;
	this->channel_lookup[15]=120;

	for (unsigned short i=1;i<8;i++)
	{
		for (unsigned short j=0;j<16;j++)
		{
			this->channel_lookup[(i<<4)+j] = this->channel_lookup[j]+i;
		}
	}
}

void raw_data::process_raw_adc(unsigned int global_event_cnt)
{
	//initialize data structure
	// for (int i = 0; i < this->save_event_header.subevents; i++)
	for (int i = 0; i < this->aconfig.n_subevents; i++)
		
	{
		std::vector<int> dummyadc(this->save_event_header.number_apvs * APV_CHANNELS, 0);
		this->myraweventdata.subevent_data.push_back(dummyadc);
	}
	
	//sample where to stop header search
	unsigned short hs_stopsearch = this->aconfig.n_adc_samples - APV_CHANNELS; 
	if (HS_STOPSEARCH<hs_stopsearch)
	{
		hs_stopsearch=HS_STOPSEARCH;
	}
	
	for (int i = 0; i < this->save_event_header.number_apvs; i++)
	{
		unsigned short liveticks_min = 65535;
		unsigned short liveticks_max = 0;
		unsigned char subevent = 0;

		// calculate the top and bottom level of the APV tickmarks
		for (int j=0; j<HS_LIVESCAN; j++)
		{
			if (this->adc_rawdata[i][j] > liveticks_max)
			{
				liveticks_max = this->adc_rawdata[i][j];
			}
			if (this->adc_rawdata[i][j] < liveticks_min)
			{
				liveticks_min = this->adc_rawdata[i][j];
			}
		}
		unsigned short liveticks_mean=(liveticks_min+liveticks_max)/2;
		unsigned short s = HS_STARTSEARCH; // set buffer to start of header

		while ((subevent < this->save_event_header.subevents) && (s < hs_stopsearch) && ((liveticks_max - liveticks_min) >= HS_MINTICKAMP))
		{
			int thesubevent = static_cast<int>(subevent);
			// search for APV frame header = three consecutive tick marks
			while ((s < hs_stopsearch) && ((this->adc_rawdata[i][s] < liveticks_mean) || (this->adc_rawdata[i][s+1] < liveticks_mean) || (this->adc_rawdata[i][s+2] < liveticks_mean))) 
			{
				s++;
			}

			if ( s < hs_stopsearch) //look for start of header and jump over it! ;-)
			{
				s+=12;
				//time to fill the analoge data
				for (int j = 0; j < APV_CHANNELS; j++)
				{
					this->myraweventdata.subevent_data.at(thesubevent).at(i * APV_CHANNELS + channel_lookup[j]) = static_cast<int>(this->adc_rawdata[i][s+j]);
				}
            s+=APV_CHANNELS;
            subevent++;
			} 
		} // here I am done with the subevents
		//fill the rest of the variables
		
	} // end of apv loop
	this->myraweventdata.event = static_cast<int>(global_event_cnt);
	this->myraweventdata.n_subevents = static_cast<int>(this->save_event_header.subevents);
	
	this->myraweventdata.subevent0_data = this->myraweventdata.subevent_data.at(0);
	this->myraweventdata.subevent1_data = this->myraweventdata.subevent_data.at(1);
	this->myraweventdata.subevent2_data = this->myraweventdata.subevent_data.at(2);
	this->myraweventdata.subevent3_data = this->myraweventdata.subevent_data.at(3);
	this->myraweventdata.subevent4_data = this->myraweventdata.subevent_data.at(4);
	this->myraweventdata.subevent5_data = this->myraweventdata.subevent_data.at(5);
}

double raw_data::convert_tdc_value ()
/* convert the read out TDC value from [ADC] to [ns] */
{
    double tdcval;
	// convert TDC
    if (this->save_event_header.eventcnt >= static_cast<unsigned int>(this->aconfig.n_pedenoiseevents))
    {
  	   // read trigger time (TDC)
       tdcval=neco_trgtime2ns(this->save_event_header.trg_time, this->aconfig.clk);
	   if (tdcval==-999.9)
	   {
	      tdcval=0.0;
	   }
	   else
	   {
	      tdcval+=this->aconfig.clk;
	   }
	}		
	else
	{
	   tdcval = 0.0;
	}
    return tdcval;
}

//time conversion
double neco_trgtime2ns(unsigned char trg_time, int sysclock)
/* converts the raw trg_time value into nanoseconds, ignoring a possible ambiguity.
   (in that case, the first occurence of a trigger signal counts.)
   The unit for time measurement is clock.now()/6 [ns], or 4.2ns in case of 40MHz.
   If trg_time does not contain a valid measurement, this function returns -999.9.
   Otherwise, the result is >=0.0 */
{
   unsigned char coarse;
   unsigned char fine;
   double ns=0;

   coarse=(trg_time&0x38)>>3;
   fine=trg_time&7;
   if (coarse && fine)
   {
      ns=sysclock*(coarse-1)+((double)sysclock)/6.0*(fine-1);
   }
   else
   {
	  ns=-999.9;
   }

   return (ns);
}
