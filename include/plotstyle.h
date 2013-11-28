//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#ifndef plotstyle_h_
#define plotstyle_h_

#include <TStyle.h>
#include <TColor.h>

#include <string>
#include <iostream>

void set_plotstyle();

Color_t LC(const unsigned int& plotIndex);

#endif