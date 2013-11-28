//-----------------------------------------------------------------------
//
//	Convoluted Landau and Gaussian Fitting Function
//         (using ROOT's Landau and Gauss functions)
//
//  Based on a Fortran code by R.Fruehwirth (fruhwirth@hephy.oeaw.ac.at)
//  Adapted for C++/ROOT by H.Pernegger (Heinz.Pernegger@cern.ch) and
//   Markus Friedl (Markus.Friedl@cern.ch)
//
//-----------------------------------------------------------------------


#include "langaus.h"

double langaufun(double *x, double *par) {
    
    //Fit parameters:
    //par[0]=Width (scale) parameter of Landau density
    //par[1]=Most Probable (MP, location) parameter of Landau density
    //par[2]=Total area (integral -inf to inf, normalization constant)
    //par[3]=Width (sigma) of convoluted Gaussian function
    //
    //In the Landau distribution (represented by the CERNLIB approximation),
    //the maximum is located at x=-0.22278298 with the location parameter=0.
    //This shift is corrected within this function, so that the actual
    //maximum is identical to the MP parameter.
    
    // Numeric constants
    double invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
//    double mpshift  = -0.22278298;       // Landau maximum location
    
    // Control constants
    double np = 100.0;      // number of convolution steps
    double sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
    
    // Variables
    double xx;
    double mpc;
    double fland;
    double sum = 0.0;
    double xlow,xupp;
    double step;
    double i;
    
    
    // MP shift correction
//    TF1* correction = new TF1("correction", "-0.1721 * x - 0.004351", 0, 20);
//    mpc = par[1] - correction->Eval(par[0]);
    mpc = par[1];
    
    // Range of convolution integral
    xlow = x[0] - sc * par[3];
    xupp = x[0] + sc * par[3];
    
    step = (xupp-xlow) / np;
    
    // Convolution integral of Landau and Gaussian by sum
    for(i=1.0; i<=np/2; i++) {
        xx = xlow + (i-.5) * step;
        fland = TMath::Landau(xx,mpc,par[0]) / par[0];
        sum += fland * TMath::Gaus(x[0],xx,par[3]);
        
        xx = xupp - (i-.5) * step;
        fland = TMath::Landau(xx,mpc,par[0]) / par[0];
        sum += fland * TMath::Gaus(x[0],xx,par[3]);
    }
    
    return (par[2] * step * sum * invsq2pi / par[3]);
}



TF1 *langaufit(TH1D *his, double *fitrange, double *startvalues, double *parlimitslo, double *parlimitshi, double *fitparams, double *fiterrors, double* peak, double* peak_error, double gaus_sigma/*double *ChiSqr, Int_t *NDF*/)
{
    // Once again, here are the Landau * Gaussian parameters:
    //   par[0]=Width (scale) parameter of Landau density
    //   par[1]=Most Probable (MP, location) parameter of Landau density
    //   par[2]=Total area (integral -inf to inf, normalization constant)
    //   par[3]=Width (sigma) of convoluted Gaussian function
    //
    // Variables for langaufit call:
    //   his             histogram to fit
    //   fitrange[2]     lo and hi boundaries of fit range
    //   startvalues[4]  reasonable start values for the fit
    //   parlimitslo[4]  lower parameter limits
    //   parlimitshi[4]  upper parameter limits
    //   fitparams[4]    returns the final fit parameters
    //   fiterrors[4]    returns the final fit errors
    //   ChiSqr          returns the chi square
    //   NDF             returns ndf
  
    Char_t FunName[100];
    
    sprintf(FunName,"Fitfcn_%s",his->GetName());
    
    TF1 *ffitold = (TF1*)gROOT->GetListOfFunctions()->FindObject(FunName);
    if (ffitold) delete ffitold;
    
    TF1 *ffit = new TF1(FunName,langaufun,fitrange[0],fitrange[1],4);
    ffit->SetParameters(startvalues);
    ffit->SetParNames("Width","MP","Area","GSigma");
    
    for (int i=0; i<4; i++) {
        ffit->SetParLimits(i, parlimitslo[i], parlimitshi[i]);
    }
    if (gaus_sigma != 0) ffit->FixParameter(3,gaus_sigma);
    if (his->GetEntries() !=0) {
        TFitResultPtr r = his->Fit(FunName,"RQNSM"); // fit within specified range, use ParLimits, do not plot
        TMatrixT<double> cov(4,4);
        cov = r->GetCovarianceMatrix(); //get covariance matrix
        
        double par_var, part_der[4];
        ffit->GetParameters(fitparams);    // obtain fit parameters
        
        TF1 *parfit = new TF1("variationfit",langaufun,fitrange[0],fitrange[1],4);
        parfit->SetParameters(fitparams);     //fit with no parameter varied
        
        double maxpeak = parfit->GetMaximumX(0, 48000); // get distribution peak

        for (int i=0; i<4; i++) {
            fiterrors[i]=ffit->GetParError(i);
            parfit->SetParameters(fitparams);        // original parameters
            double delta_par = fitparams[i] * 0.001; // the delta for the variation
            par_var = fitparams[i] + delta_par;      // new vector with one par varied
            parfit->SetParameter(i, par_var);        // fit with i-th parameter varied
            double peakpos = parfit->GetMaximumX(0, 48000);

            part_der[i] = (peakpos-maxpeak)/delta_par; // compute i-th partial derivative
        }
        delete parfit;
        
        //matrix multiplication
        double error_2=0, peakerror=0;
        for (int i = 0; i < 4; i++) {
            double temp = 0;
            for (int j =0; j < 4; j++) {
                temp+=cov(i,j)*part_der[j];
            }
            error_2 += temp * part_der[i];
        }
        peakerror = sqrt(error_2);
        *peak_error = peakerror;
        *peak = maxpeak;
    }
    return (ffit);
}


TF1* langaus(TH1D* hSNR, double& peak, double& peak_error, double& mpv, double& mpv_error, double gaus_sigma) {
    
    // Setting fit range and start values
    double fr[2];
    double sv[4], pllo[4], plhi[4];
    double fp[4], fpe[4];
    
    fr[0] = 0;
    fr[1] = 10.0*hSNR->GetMean();
    
    pllo[0]=0.5; pllo[1]=2.0; pllo[2]=1.0; pllo[3]=0.1;
    plhi[0]=15.0; plhi[1]=100.0; plhi[2]=1000000.0; plhi[3]=15.0;
    sv[0]=1.8; sv[1]=20.0; sv[2]=50000.0; sv[3]=3.0;
    
    TF1 *fitsnr = langaufit(hSNR,fr,sv,pllo,plhi,fp,fpe,&peak,&peak_error, gaus_sigma);
    
    mpv = fitsnr->GetParameter(1);
    mpv_error = fitsnr->GetParError(1);
    
    if (peak_error > 3) {
        peak_error = 0; //zero error is dangerous
    }
    return fitsnr;
}
