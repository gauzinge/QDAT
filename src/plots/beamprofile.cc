//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

#include "plots.h"

void beamprofile(settings* mysettings)
{
	set_plotstyle();
	
	config myconfig;
	myconfig.read_config_silent(mysettings);
	
	cluster mycluster(&myconfig);
	mycluster.read_file();
	
		for (std::map<std::string, modulesettings>::iterator it =  mysettings->modulemap.begin(); it != mysettings->modulemap.end(); it++)
		{
			module* amodule = myconfig.find_module(it->first);
		
			std::cout << "PLOT Creating Beamprofile for " << it->first << std::endl;
			
			std::stringstream plotname;
			plotname << amodule->name << " Beam Profile";
			
			TH1D* beamprofile = new TH1D (plotname.str().c_str(), plotname.str().c_str(),amodule->sensor_strips,amodule->first_strip,amodule->last_strip);
			
			for (int event = 0; event < mycluster.get_n_entries(); event++)
			{
				mycluster.get_entry(event);
				if (mycluster.modulename == amodule->name) beamprofile->Fill(mycluster.cog);
			}
			
			beamprofile->GetXaxis()->SetTitle("Strip Number");
			
			std::stringstream obname;
			obname << amodule->name << "_beamprofile";
			TCanvas* canvas = new TCanvas(obname.str().c_str(), plotname.str().c_str());
			canvas->cd();
			beamprofile->Draw();
			
			
			myconfig.plotfile->WriteObject(canvas,obname.str().c_str());
			std::cout << "Finished creating Beamprofiles!" << std::endl;
	}
}