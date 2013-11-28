//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

#include "plots.h"

void clusterwidth(settings* mysettings)
{
	set_plotstyle();
	
	config myconfig;
	myconfig.read_config_silent(mysettings);
	
	cluster mycluster(&myconfig);
	mycluster.read_file();
	
		for (std::map<std::string, modulesettings>::iterator it =  mysettings->modulemap.begin(); it != mysettings->modulemap.end(); it++)
		{
			module* amodule = myconfig.find_module(it->first);
		
			std::cout << "PLOT Creating Clusterwidth Plot for " << it->first << std::endl;
			
			//normal
			std::stringstream plotname_good;
			plotname_good << amodule->name << " Clusterwidth_good";
			TH1D* clusterwidth_good = new TH1D (plotname_good.str().c_str(), plotname_good.str().c_str(),10,0.5,10.5);
			
			//special ROI
			std::stringstream plotname_bad;
			plotname_bad << amodule->name << " Clusterwidth_bad (" << it->second.low_ROI << "," << it->second.high_ROI << ")";
			TH1D* clusterwidth_bad = new TH1D (plotname_bad.str().c_str(), plotname_bad.str().c_str(),10,0.5,10.5);
			
			for (int event = 0; event < mycluster.get_n_entries(); event++)
			{
				mycluster.get_entry(event);
				if (mycluster.modulename == amodule->name)
					{
						if (mysettings->in_roi(static_cast<int>(mycluster.cog),mycluster.modulename))
						{
							clusterwidth_bad->Fill(mycluster.str_signals->size());
						}
						else
						{
							clusterwidth_good->Fill(mycluster.str_signals->size());
						}
					} 
			}
			
			clusterwidth_good->GetXaxis()->SetTitle("Clusterwidth");
			clusterwidth_bad->GetXaxis()->SetTitle("Clusterwidth");
			std::stringstream plotname;
			plotname << it->first << " Clusterwidth";
			std::stringstream obname;
			obname << amodule->name << "_clusterwidth";
			TCanvas* canvas = new TCanvas(obname.str().c_str(), plotname.str().c_str());
			canvas->cd();
			if (clusterwidth_bad->GetEntries() > 200)
			{
				canvas->Divide(1,2);
				canvas->cd(1);
				clusterwidth_good->Draw();
				canvas->cd(2);
				clusterwidth_bad->Draw();
			}
			else clusterwidth_good->Draw();
			
			
			myconfig.plotfile->WriteObject(canvas,obname.str().c_str(),"Overwrite");
			std::cout << "Finished creating Clusterwidth Histogram!" << std::endl;
	}
}