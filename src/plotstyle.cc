//
//  written by G. Auzinger 
//	CERN Sept. 2013
//


#include "plotstyle.h"

void set_plotstyle() {
    gStyle->SetOptStat(1000001110);
    gStyle->SetOptFit(0011);
    gStyle->SetHistLineWidth(2);
    gStyle->SetFrameLineWidth(3);
    gStyle->SetLabelFont(42,"xyz");
    gStyle->SetLabelSize(0.05,"xyz");
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetTitleSize(0.05,"xyz");
    gStyle->SetTitleFont(62,"xyz");
    gStyle->SetTitleOffset(1,"yz");
    gStyle->SetFuncColor(2);
    gStyle->SetFuncWidth(3);
    
    gStyle->SetFillStyle(0);
    gStyle->SetLegendBorderSize(0);
    gStyle->SetMarkerSize(2);
    gStyle->SetCanvasColor(0);
}

Color_t LC(const unsigned int& plotIndex)  //Libre Office Color Scheme
{
    std::string colorCode;
    if (plotIndex==0) colorCode = "#000000";
    else {
        int nColor=(plotIndex-1) % 12;
        switch (nColor) {
            case 0 :
                colorCode="#014586";
                break;
            case 1 :
                colorCode="#FF420E";
                break;
            case 2 :
                colorCode="#FFD320";
                break;
            case 3 :
                colorCode="#579D1C";
                break;
            case 4 :
                colorCode="#7E0021";
                break;
            case 5 :
                colorCode="#83CAFF";
                break;
            case 6 :
                colorCode="#314004";
                break;
            case 7 :
                colorCode="#AECF00";
                break;
            case 8 :
                colorCode="#4B1F6F";
                break;
            case 9 :
                colorCode="#FF950E";
                break;
            case 10 :
                colorCode="#C5000B";
                break;
            case 11 :
                colorCode="#0084D1";
                break;
            default :
                std::cerr << "ERROR: in Vizard::getNiceColor() n%12 is not an int between 0 and 11! This should not happen." << std::endl;
                colorCode="#000000";
                break;
        }
    }
    return TColor::GetColor(colorCode.c_str());
}
