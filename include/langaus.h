

#ifndef _langaus_h
#define _langaus_h

#include "TH1.h"
#include "TF1.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TMath.h"
#include "TMatrixT.h"
#include "TFitResult.h"

#include "definitions.h"

double langaufun(double* x, double* par);

TF1 *langaufit(TH1D *his, double *fitrange, double *startvalues, double *parlimitslo, double *parlimitshi, double *fitparams, double *fiterrors, double* peak, double* peak_error, double gaus_sigma);

TF1* langaus(TH1D* hSNR, double& peak, double& peak_error, double& mpv, double& mpv_error, double gaus_sigma);

#endif
