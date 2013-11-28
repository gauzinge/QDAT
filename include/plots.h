//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#ifndef plots_h_
#define plots_h_

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <iomanip>

//CUSTOM
#include "plotstyle.h"
#include "definitions.h"
#include "settings.h"
#include "config.h"
#include "pedenoise.h"
#include "noisemap.h"
#include "calibration.h"
#include "data.h"
#include "cluster.h"
#include "langaus.h"

//ROOT
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLatex.h>
#include <TText.h>
#include <TLegend.h>

void noiseprofile(settings* mysettings);

void beamprofile(settings* mysettings);

void clusterwidth(settings* mysettings);

void eta(settings* mysettings);

void signal(settings* mysettings);

void signal_to_noise(settings* mysettings);

void hitmap_2D(settings* mysettings);

#endif