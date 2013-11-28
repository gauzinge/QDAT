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
#include "plots.h"


void plot(std::string filename) {
		
	settings mysettings(filename);
	mysettings.read_plot_settings();
	
	if (mysettings.noiseprofile) noiseprofile(&mysettings);
	if (mysettings.beamprofile) beamprofile(&mysettings);
	if (mysettings.clusterwidth) clusterwidth(&mysettings);
	if (mysettings.eta) eta(&mysettings);
	if (mysettings.signal) signal(&mysettings);
	if (mysettings.snr) signal_to_noise(&mysettings);
	//Experimental feature, needs more work
	// hitmap_2D(&mysettings);
	
	std::cout << "Finished plotting, Canvases saved, quit with CTRL+C!" << std::endl;
 }

void syntax(char* progname) {
    std::cout << "Syntax: " << progname << " Settingfile" << std::endl;
}

void StandaloneApplication(int argc, char** argv) {
    if (argc<1) {
        syntax(argv[0]);
    }
    char* filename = argv[1];
    // std::string filename = "Data/run007.cfg";
    plot(filename);
}

int main(int argc, char** argv) {
    gROOT->Reset();
    TApplication app("Root Application", &argc, argv);
    StandaloneApplication(app.Argc(), app.Argv());
    app.Run();
    return 0;
}