//
//  written by G. Auzinger 
//	CERN Sept. 2013
//

#include "plots.h"

void signal_to_noise(settings* mysettings)
{
	set_plotstyle();
		
	config myconfig;
	myconfig.read_config_silent(mysettings);
	
	cluster mycluster(&myconfig);
	mycluster.read_file();
	
		for (std::map<std::string, modulesettings>::iterator it =  mysettings->modulemap.begin(); it != mysettings->modulemap.end(); it++)
		{
			module* amodule = myconfig.find_module(it->first);
		
			std::cout << "PLOT Creating Signal-to-Noise Plot for " << it->first << std::endl;
			
			std::vector<TH1D*> snr_good;
			std::vector<TH1D*> snr_bad;
			
		    TLegend* legend =  new TLegend(0.14,0.7,0.43,0.88,"","NDC");
			
			//normal
			for (int i = 0; i <3; i++)
			{
				std::stringstream plotname_good;
				plotname_good << amodule->name << " SNR_good";
				std::stringstream histoname_good;
				histoname_good << amodule->name << "_" << i << "Strip";
				TH1D* dummy = new TH1D (histoname_good.str().c_str(), plotname_good.str().c_str(),80,0,80);
				snr_good.push_back(dummy);
			}
			//special ROI
			for (int i = 0; i <3; i++)
			{
				std::stringstream plotname_bad;
				plotname_bad << amodule->name << " SNR_bad (" << it->second.low_ROI << "," << it->second.high_ROI << ")";
				std::stringstream histoname_bad;
				histoname_bad << amodule->name << "_" << i << "Strip (" << it->second.low_ROI << "," << it->second.high_ROI << ")";
				TH1D* dummy = new TH1D (histoname_bad.str().c_str(), plotname_bad.str().c_str(),80,0,80);
				snr_bad.push_back(dummy);
			}
		
			for (int event = 0; event < mycluster.get_n_entries(); event++)
			{
				mycluster.get_entry(event);
				if (mycluster.modulename == amodule->name)
					{
						if (mysettings->in_roi(static_cast<int>(mycluster.cog),mycluster.modulename))
						{
							if (mycluster.str_signals->size() == 1) snr_bad.at(0)->Fill(mycluster.signal / mycluster.noise);
							else if (mycluster.str_signals->size() == 2) snr_bad.at(1)->Fill(mycluster.signal / mycluster.noise);
							else if (mycluster.str_signals->size() >= 3) snr_bad.at(2)->Fill(mycluster.signal / mycluster.noise);
						}
						else
						{
							if (mycluster.str_signals->size() == 1) snr_good.at(0)->Fill(mycluster.signal / mycluster.noise);
							else if (mycluster.str_signals->size() == 2) snr_good.at(1)->Fill(mycluster.signal / mycluster.noise);
							else if (mycluster.str_signals->size() >= 3) snr_good.at(2)->Fill(mycluster.signal / mycluster.noise);
						}
					} 
			}
			
			for (int i=0; i <3; i++)
			{
				int color;
				if (i<2) color = i+1;
				else color = 4;
				snr_good.at(i)->GetXaxis()->SetTitle("Signal-to-Noise");
				snr_good.at(i)->SetLineColor(LC(color));
				snr_good.at(i)->SetStats(kFALSE);
				
				std::stringstream legendentry;
				if (i < 2) legendentry << "SNR Clusterwidth   " << i+1;
				else legendentry << "SNR Clusterwidth >= 3";
				legend->AddEntry(snr_good.at(i),legendentry.str().c_str(),"l");
				
				snr_bad.at(i)->GetXaxis()->SetTitle("Signal-to-Noise");
				snr_bad.at(i)->SetLineColor(LC(color));
				snr_bad.at(i)->SetStats(kFALSE);
			}
			
			std::stringstream plotname;
			plotname << it->first << " Signal-to-Noise";
			std::stringstream obname;
			obname << amodule->name << "_SNR";
			TCanvas* canvas = new TCanvas(obname.str().c_str(), plotname.str().c_str());
			canvas->cd();
			if (snr_bad.at(0)->GetEntries() + snr_bad.at(1)->GetEntries() + snr_bad.at(2)->GetEntries() > 300)
			{
				canvas->Divide(1,2);
				for (int i = 0; i <3; i++)
				{
					std::string option;
					if (i == 0) option = "";
					else option = "same";
					
					canvas->cd(1);
					snr_good.at(i)->Draw(option.c_str());
					legend->Draw("same");
					
					double mpv_good, mpv_error_good, landau_mpv_good, landau_mpv_error_good;
					langaus(snr_good.at(i), mpv_good, mpv_error_good, landau_mpv_good, landau_mpv_error_good,0);
					
					if (i<2)
					{
						std::stringstream goodmpvstream;
						goodmpvstream << std::setprecision(2) << "SNR  " << i+1 << " Strip = " << mpv_good;
						// else goodmpvstream << std::setprecision(2) << "SNR >3 Strip = " << mpv_good;
						TLatex* goodmpv = new TLatex(.6,.85-i*.06,goodmpvstream.str().c_str());
						goodmpv->SetNDC(kTRUE); // <- use NDC coordinate
						goodmpv->SetTextFont(62);
						goodmpv->SetTextSize(0.045);
						goodmpv->Draw("same");
					}
					
					canvas->cd(2);
					snr_bad.at(i)->Draw(option.c_str());
					double mpv_bad, mpv_error_bad, landau_mpv_bad, landau_mpv_error_bad;
					langaus(snr_bad.at(i), mpv_bad, mpv_error_bad, landau_mpv_bad, landau_mpv_error_bad,0);
					
					if (i<2)
					{
						std::stringstream badmpvstream;
						badmpvstream << std::setprecision(2) << "SNR  " << i+1 << " Strip = " << mpv_good;
						TLatex* badmpv = new TLatex(.6,.85-i*.06,badmpvstream.str().c_str());
						badmpv->SetNDC(kTRUE); // <- use NDC coordinate
						badmpv->SetTextFont(62);
						badmpv->SetTextSize(0.045);
						badmpv->Draw("same");
					}
				}
			}
			else 
			{
				for (int i = 0; i <3; i++)
				{
					std::string option;
					if (i == 0) option = "";
					else option = "same";
					snr_good.at(i)->Draw(option.c_str());
					legend->Draw("same");
					
					double mpv_good, mpv_error_good, landau_mpv_good, landau_mpv_error_good;
					langaus(snr_good.at(i), mpv_good, mpv_error_good, landau_mpv_good, landau_mpv_error_good,0);

					if (i<2)
					{
						std::stringstream goodmpvstream;
						goodmpvstream << std::setprecision(2) << "SNR  " << i+1 << " Strip = " << mpv_good;
						TLatex* goodmpv = new TLatex(.6,.85-i*.06,goodmpvstream.str().c_str());
						goodmpv->SetNDC(kTRUE); // <- use NDC coordinate
						goodmpv->SetTextFont(62);
						goodmpv->SetTextSize(0.045);
						goodmpv->Draw("same");	
					}
				}
			}
			
			
			myconfig.plotfile->WriteObject(canvas,obname.str().c_str(),"Overwrite");
			std::cout << "Finished creating Signal-to-Noise Histogram!" << std::endl;
	}
}