//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

#include "plots.h"

void eta(settings* mysettings)
{
	set_plotstyle();
	
	config myconfig;
	myconfig.read_config_silent(mysettings);
	
	cluster mycluster(&myconfig);
	mycluster.read_file();
	
		for (std::map<std::string, modulesettings>::iterator it =  mysettings->modulemap.begin(); it != mysettings->modulemap.end(); it++)
		{
			module* amodule = myconfig.find_module(it->first);
		
			std::cout << "PLOT Creating Eta Distribution Plot for " << it->first << std::endl;
			
			//normal
			std::stringstream plotname_good;
			plotname_good << amodule->name << " Eta_good";
			TH1D* eta_good = new TH1D (plotname_good.str().c_str(), plotname_good.str().c_str(),200,-.1,1.1);
			eta_good->SetStats(kFALSE);
			//special ROI
			std::stringstream plotname_bad;
			plotname_bad << amodule->name << " Eta_bad (" << it->second.low_ROI << "," << it->second.high_ROI << ")";
			TH1D* eta_bad = new TH1D (plotname_bad.str().c_str(), plotname_bad.str().c_str(),200,-.1,1.1);
			eta_bad->SetStats(kFALSE);
			for (int event = 0; event < mycluster.get_n_entries(); event++)
			{
				mycluster.get_entry(event);
				if (mycluster.modulename == amodule->name)
					{
						if (mysettings->in_roi(static_cast<int>(mycluster.cog),mycluster.modulename))
						{
							eta_bad->Fill(mycluster.eta);
						}
						else
						{
							eta_good->Fill(mycluster.eta);
						}
					} 
			}
			
			eta_good->GetXaxis()->SetTitle("Relative Position");
			eta_bad->GetXaxis()->SetTitle("Relative Position");
			std::stringstream plotname;
			plotname << it->first << " Eta Distribution";
			std::stringstream obname;
			obname << amodule->name << "_eta";
			TCanvas* canvas = new TCanvas(obname.str().c_str(), plotname.str().c_str());
			canvas->cd();
			if (eta_bad->GetEntries() > 200)
			{
				canvas->Divide(1,2);
				canvas->cd(1);
				eta_good->Draw();
				canvas->cd(2);
				eta_bad->Draw();
			}
			else eta_good->Draw();
			
			
			myconfig.plotfile->WriteObject(canvas,obname.str().c_str(),"Overwrite");
			std::cout << "Finished creating Eta Distribution Histogram!" << std::endl;
	}
}