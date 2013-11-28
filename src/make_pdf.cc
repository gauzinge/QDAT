#include <TColor.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TKey.h>
#include <TList.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TSystemDirectory.h>
#include <TPad.h>
#include <TGraphErrors.h>
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>

void make_pdf(std::string filename)
{	
	gStyle->SetCanvasDefH(500);
	gStyle->SetCanvasDefW(700);
	std::string foldername = filename.substr(0, filename.length()-5);
	mkdir(foldername.c_str(),S_IRWXU | S_IRWXG | S_IRWXO );
	
	TFile* fileIn = TFile::Open(filename.c_str(), "READ");
	TList* listOfKeys = fileIn->GetListOfKeys();
	TCanvas* myCanvas = NULL;
	if (listOfKeys) {
		std::cout << "Found " << listOfKeys->GetSize() << "Keys in TFile!" << std::endl;
		for (int i=0; i<listOfKeys->GetSize(); ++i) 
		{
			TKey* aKey = (TKey*) listOfKeys->At(i);
			if ((aKey)&&(strcmp(aKey->GetClassName(),"TCanvas")==0)) {
				myCanvas = (TCanvas*)aKey->ReadObj();
				std::string outpdfName = foldername + "/" + static_cast<std::string>(myCanvas->GetName()) + ".pdf";
				std::cout << i+1 << " Saving as " << outpdfName << std::endl;
				myCanvas->SaveAs(outpdfName.c_str());
			}
		}
	}
	if (!myCanvas) {
		std::cerr << "Error: could not find any canvas in the file" << std::endl;
	}
}

void syntax(char* progname) {
    std::cout << "Syntax: " << progname << " Plotfile" << std::endl;
}

int main(int argc, char** argv)
{
	if (argc<1) syntax(argv[0]);
	make_pdf(argv[1]);
	return 0;
}
