readme.txt

Quick and Dirty Analysis Tool
by G. Auzinger, CERN 2013
georg.auzinger@cern.ch

Disclaimer:
This software was developed during a project for HEPHY. Feel free to use, modify and change it. Also, the author can not exclude the presence of bugs that have not shown up at the time. If results for publication are created with this tool, the author would be happy to be mentioned.

Install:
simply extract src/ and include/ directorys to the working directory, create a Settings/ directory for the settings files (see example.xml)
Create a Results/  and lib/ directory 

In the settings file specify Data/ CalibrationData/ and configfile paths and the respective run numbers (see comments in example.xml)

WARNING: calibration files have to be copied to a local directory and manually edited to remove the block at the end of the file as QDAT can not read the file like this.

Compilation:
written on 64bit Unix, be careful with datatypes on WIN32 systems (change unsigned int to unsigned long in T_save_event_header struct in raw_data.h).
Requires a recent install of ROOT (www.root.cern.ch) and an up-to-date version of gcc that supports C++0x features.
invoke compilation by runnig "make"

Use:
run eigher "./process Settingdir/settingfile.xml" to run the data-processing specified in settingfile.xml for one file or "./submit_process.sh Settingdir/settingfile*" to process several runs in a batched job.
Low level results (pedenoise profiles, HTR plots) will be stored in Resultpath/Results_runXXX.root and the intermediate data collections will be saved in the Data/ directory specified in the setting file.

To create plots, run "./plot Settings/settingfile.xml" which will launch an interactive ROOT session with editable interactive canvasses.
If all plots have been modified for convenience, save them manually and quit the application with "CTRL+C" The initial versions of the plots will be automatically saved to Resultdir/Plots_runXXX.root.

To create .pdf files from the plots saved in this file, run "./make_pdf Resultdir/Plots_runXXX.root" which will create a directory for the run in the Resultdir/ folder.

2D Hitmap:
this is an experimental feature that was not developed further due to lack of time. However, it outlines a way to find all clusters within a specific event from the cluster collection and store them in a vector of clusters. Then, by defining reasonable criteria for hit correlation, it is in principle possible to create a 2D hitmap as it is implemented right now. The present version however, does not distinguis between hits on the top and bottom part of the strixel. This selection and correlatino algorithm has to be refined. Have fun who ever is up to it!