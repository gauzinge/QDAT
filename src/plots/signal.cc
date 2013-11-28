//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

#include "plots.h"

void signal(settings* mysettings)
{
	set_plotstyle();
	
	config myconfig;
	myconfig.read_config_silent(mysettings);
	
	cluster mycluster(&myconfig);
	mycluster.read_file();
	
	calibration mycalibration(&myconfig);
	mycalibration.load_calibration();
	
		for (std::map<std::string, modulesettings>::iterator it =  mysettings->modulemap.begin(); it != mysettings->modulemap.end(); it++)
		{
			module* amodule = myconfig.find_module(it->first);
		
			std::cout << "PLOT Creating Signal Histogram for " << it->first << std::endl;
			
			//normal
			std::stringstream plotname_good;
			plotname_good << amodule->name << " Signal_good";
			TH1D* signal_good = new TH1D (plotname_good.str().c_str(), plotname_good.str().c_str(),200,0,80);
			signal_good->SetStats(kFALSE);
			
			//special ROI
			std::stringstream plotname_bad;
			plotname_bad << amodule->name << " Signal_bad (" << it->second.low_ROI << "," << it->second.high_ROI << ")";
			TH1D* signal_bad = new TH1D (plotname_bad.str().c_str(), plotname_bad.str().c_str(),200,0,80);
			signal_bad->SetStats(kFALSE);
			for (int event = 0; event < mycluster.get_n_entries(); event++)
			{
				mycluster.get_entry(event);
				if (mycluster.modulename == amodule->name)
				{
					int global_strip = amodule->global_first_strip + mycluster.first_strip; //sum strip signal of cluster and calibrate with global_strip
					double signal = 0;
					for(unsigned int clustrips = 0; clustrips < mycluster.str_signals->size(); clustrips++)
					{
						signal += mycluster.str_signals->at(clustrips) * mycalibration.calfactor->at(global_strip + clustrips); //this is the global first strip of the cluster + loc 
					}
					signal /= 1000; //signal in ke-
					if (mysettings->in_roi(static_cast<int>(mycluster.cog),mycluster.modulename))
					{
						signal_bad->Fill(signal);
					}
					else
					{
						signal_good->Fill(signal);
					}
				} 
			}
			
			signal_good->GetXaxis()->SetTitle("Signal [ke^{-}]");
			signal_bad->GetXaxis()->SetTitle("Signal [ke^{-}]");
			std::stringstream plotname;
			plotname << it->first << " Signal";
			std::stringstream obname;
			obname << amodule->name << "_signal";
			TCanvas* canvas = new TCanvas(obname.str().c_str(), plotname.str().c_str());
			canvas->cd();
	        gStyle->SetOptStat("emr");
			if (signal_bad->GetEntries() > 200)
			{
				canvas->Divide(1,2);

				canvas->cd(1);
				double mpv_good, mpv_error_good, landau_mpv_good, landau_mpv_error_good;
				TF1* fit_good = langaus(signal_good, mpv_good, mpv_error_good, landau_mpv_good, landau_mpv_error_good,0);
				signal_good->Draw();
				fit_good->Draw("same");
				
				std::stringstream goodmpvstream;
				goodmpvstream << std::setprecision(4) << "MPV=" << mpv_good << "#pm" << std::setprecision(3) << mpv_error_good << "ke^{-}";
				TLatex* goodmpv = new TLatex(.55,.8,goodmpvstream.str().c_str());
				goodmpv->SetNDC(kTRUE); // <- use NDC coordinate
				goodmpv->SetTextFont(62);
				goodmpv->SetTextSize(0.055);
				goodmpv->Draw("same");
				
				canvas->cd(2);
				double mpv_bad, mpv_error_bad, landau_mpv_bad, landau_mpv_error_bad;
				TF1* fit_bad = langaus(signal_bad, mpv_bad, mpv_error_bad, landau_mpv_bad, landau_mpv_error_bad,0);
				signal_bad->Draw();
				fit_bad->Draw("same");
				
				std::stringstream badmpvstream;
				badmpvstream << std::setprecision(4)  << "MPV=" << mpv_bad << "#pm" << std::setprecision(3) << mpv_error_bad << "ke^{-}";
				TLatex* badmpv = new TLatex(.55,.8,badmpvstream.str().c_str());
				badmpv->SetNDC(kTRUE); // <- use NDC coordinate
				badmpv->SetTextFont(62);
				badmpv->SetTextSize(0.055);
				badmpv->Draw("same");
			}
			else 
			{
				double mpv_good, mpv_error_good, landau_mpv_good, landau_mpv_error_good;
				TF1* fit_good = langaus(signal_good, mpv_good, mpv_error_good, landau_mpv_good, landau_mpv_error_good,0);
				signal_good->Draw();
				fit_good->Draw("same");
				std::stringstream goodmpvstream;
				goodmpvstream << std::setprecision(4)  << "MPV=" << mpv_good << "#pm" << std::setprecision(3) << mpv_error_good << "ke^{-}";
				TLatex* goodmpv = new TLatex(.55,.8,goodmpvstream.str().c_str());
				goodmpv->SetNDC(kTRUE); // <- use NDC coordinate
				goodmpv->SetTextFont(62);
				goodmpv->SetTextSize(0.055);
				goodmpv->Draw("same");
			}
			
			myconfig.plotfile->WriteObject(canvas,obname.str().c_str(),"Overwrite");
			std::cout << "Finished creating Eta Distribution Histogram!" << std::endl;
	}
}