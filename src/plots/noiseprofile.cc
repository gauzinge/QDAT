//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

#include "plots.h"

void noiseprofile(settings* mysettings)
{
	set_plotstyle();
	config myconfig;
	myconfig.read_config_silent(mysettings);
	
	pedenoise mypedenoise(&myconfig);
	mypedenoise.read_pedenoise();
	
	noisemap mynoisemap(&myconfig);
	mynoisemap.load_noisemap();
	
	calibration mycalibration(&myconfig);
	mycalibration.load_calibration();
	
		for (std::map<std::string, modulesettings>::iterator it =  mysettings->modulemap.begin(); it != mysettings->modulemap.end(); it++)
		{
			module* amodule = myconfig.find_module(it->first);
		
			std::cout << "PLOT Creating Noiseprofile for " << it->first << std::endl;
			//fill
			TGraph* noisegr = new TGraph();
			noisegr->SetMarkerColor(LC(1));
			noisegr->SetMarkerStyle(20);
			noisegr->SetMarkerSize(.6);
			TGraph* badstripgr = new TGraph();
			badstripgr->SetMarkerColor(LC(2));
			badstripgr->SetMarkerStyle(20);
			badstripgr->SetMarkerSize(.6);
			
			unsigned int local_strip = amodule->first_strip;
			
			for (int strip = amodule->global_first_strip; strip < amodule->global_first_strip + amodule->last_strip; strip++)
			{
				if (!mynoisemap.strip_noisy(static_cast<int>(strip))) //good strip
					//maybe debug and check global strip vs noisemap manually
				{
					noisegr->SetPoint(noisegr->GetN(),local_strip,mycalibration.calfactor->at(strip) * mypedenoise.noise->at(strip)/1000);
				}
				else
				{
					badstripgr->SetPoint(badstripgr->GetN(),local_strip,mycalibration.calfactor->at(strip) * mypedenoise.noise->at(strip)/1000);
				}
				local_strip++;
			}
			TMultiGraph* multigraph = new TMultiGraph();
			multigraph->Add(noisegr,"p");
			multigraph->Add(badstripgr,"p");
			std::stringstream plotname;
			plotname << amodule->name << " NoiseProfile;Strip Number;Noise [ke^{-}]";
			multigraph->SetTitle(plotname.str().c_str());
			std::stringstream obname;
			obname << amodule->name << "_noiseprofile";
			TCanvas* canvas = new TCanvas(obname.str().c_str(), plotname.str().c_str());
			canvas->cd();
			multigraph->Draw("A");
			
			
			myconfig.plotfile->WriteObject(canvas,obname.str().c_str(),"Overwrite");
			std::cout << "Finished creating Noiseprofiles!" << std::endl;
	}
}