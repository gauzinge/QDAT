//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

//ROOT
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"

#include "plotstyle.h"
#include "settings.h"
#include "config.h"
#include "calibration.h"
#include "read_raw.h"
#include "pedenoise.h"
#include "noisemap.h"
#include "data.h"
#include "cluster.h"

void syntax(char* progname) {
    std::cout << "Syntax: " << progname << " Settingfile" << std::endl;
}

int main(int argc, char** argv) 
{
	
	if (argc<1) {
		syntax(argv[0]);
	}
	
	set_plotstyle();
	
	settings mysettings(argv[1]);
	mysettings.read_process_settings();
	
	config myconfig;
	myconfig.read_config(&mysettings);
	
	if (mysettings.calibrate)
	{
		calibration mycalibration(&myconfig);
		mycalibration.get_calfactors(mysettings.nominal_signal);
	}
	if (mysettings.raw_data)
	{
		raw_data myrawdata(&myconfig);
		myrawdata.read_raw(mysettings.max_events);
	}
	if (mysettings.pedenoise) 
	{	
		pedenoise mypedenoise(&myconfig);
		mypedenoise.calculate_pedenoise(mysettings.cm, mysettings.cm_group, mysettings.n_discarded_strips, mysettings.hitrejectioncut,1,mysettings.cm_loops);
	}
	if (mysettings.noisemap) 
	{
		noisemap mynoisemap(&myconfig);
		mynoisemap.find_noisestrips(mysettings.reject_cut,mysettings.stripgroup);
	}
	if (mysettings.calibrate_data) 
	{
		data mydata(&myconfig);
		mydata.process(mysettings.htr_events,mysettings.cluwindow,mysettings.seed);
	}
	if (mysettings.clustering) 
	{
		cluster mycluster(&myconfig);
		mycluster.find_clusters(mysettings.clu_seed, mysettings.clu_neighbor, mysettings.clu_total);	
	}
	std::cout << "Finished processing!" << std::endl;
	return 0;
}