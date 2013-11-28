//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#include "plots.h"

struct lwt_clu{
	std::string name;
	double cog;
};

void hitmap_2D(settings* mysettings)
{
	set_plotstyle();
	
	config myconfig;
	myconfig.read_config_silent(mysettings);
	
	cluster mycluster(&myconfig);
	mycluster.read_file();
	if (myconfig.run == 7 || myconfig.run == 8)
	{
		//upstream
		TH2D* upstream = new TH2D ("upstream", "2D hitmap upstream",256,0,256,256,0,256);
		
		//downstream
		TH2D* downstream = new TH2D ("downstream", "2D hitmap downstream",256,0,256,256,0,256);
		
		std::cout << "PLOT Creating 2D hitmap by correlating hits on the two upstream and the two downstream modules!" << std::endl;
		
		int index = 0;
		int counter = 0;
		while (index < mycluster.get_n_entries())
		{
			std::vector<lwt_clu> clu_vec;
			mycluster.get_entry(index);
			int theevent = mycluster.event;
			int event = mycluster.event;
			lwt_clu mylwtclu;
			mylwtclu.name = mycluster.modulename;
			mylwtclu.cog = mycluster.cog;
			clu_vec.push_back(mylwtclu);
			counter++;
			index++;
			while(event == theevent)
			{
				mycluster.get_entry(index);
				event = mycluster.event; //next cluster
				if (event == theevent)  //event is still theevent
				{
					lwt_clu mylwtclu;
					mylwtclu.name = mycluster.modulename;
					mylwtclu.cog = mycluster.cog;
					clu_vec.push_back(mylwtclu);
					counter++;
				}
				else counter++;
				index++; //look in next 
			}
			// std::cout << "Found " << clu_vec.size() << " Clusters in Event " << theevent << " " << counter << " " << mycluster.get_n_entries() << std::endl;
			if (counter > mycluster.get_n_entries()*.8) break;
			
			// I found all clusters with the same event number
			//FILL the histogram
			double xu, xd, yu, yd;
			xu=0;
			yu=0;
			xd=0;
			xu=0;
			for (unsigned int i = 0; i < clu_vec.size(); i++)
			{
				if (clu_vec.at(i).name == "07_baby") xu = clu_vec.at(i).cog;
				if (clu_vec.at(i).name == "strixel_2_top" || clu_vec.at(i).name == "strixel_2_bottom") yu = clu_vec.at(i).cog;
				if (clu_vec.at(i).name == "12_baby") xd = clu_vec.at(i).cog;
				if (clu_vec.at(i).name == "strixel_1_top" || clu_vec.at(i).name == "strixel_1_bottom") yd = clu_vec.at(i).cog;
			}
			if (xu != 0 && yu != 0) upstream->Fill(xu,yu);
			if (xd != 0 && yd != 0) downstream->Fill(xd, yd);
			
		}
		TCanvas* upcanvas = new TCanvas("upcanvas", "Hitmap of the Upstream Sensors");
		upcanvas->cd();
		upstream->Draw();		
			
		TCanvas* downcanvas = new TCanvas("downcanvas", "Hitmap of the Downstream Sensors");
		downcanvas->cd();
		downstream->Draw();	
		
		myconfig.plotfile->WriteObject(upcanvas,"hitmap_upstream","Overwrite");
		myconfig.plotfile->WriteObject(downcanvas,"hitmap_downstream","Overwrite");
		
		std::cout << "Finished creating 2D Hitmap!" << std::endl;
	}
	else std::cout << "No Y information available!" << std::endl;
}
