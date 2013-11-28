#!/bin/bash
# source /afs/cern.ch/sw/lcg/contrib/gcc/4.6/x86_64-slc5-gcc46-opt/setup.sh
# source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.01/x86_64-slc5-gcc46-dbg/root/bin/thisroot.sh
# export PATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.01/x86_64-slc5-gcc46-dbg/root/lib/:$PATH
# export PATH=/afs/cern.ch/sw/lcg/external/xrootd/3.2.7/x86_64-slc5-gcc46-opt/bin:$PATH
# export LD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/contrib/gcc/4.6/x86_64-slc5-gcc46-opt/lib64:${LD_LIBRARY_PATH}
# export LD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/external/xrootd/3.2.7/x86_64-slc5-gcc46-opt/lib64:${LD_LIBRARY_PATH}

#set path to rootlib, thisroot.sh, gcc, lib64 here if rquired

# mounteos
cd "$(dirname "$0")"
echo "The Working Directory is: ";
 pwd
if [[ $# -ne 0 ]];
then
echo 'Scanning for RunSetting Files';
counter=0;
for i in $*;
do
echo $i;
((counter++));
done
echo 'There are '$counter 'RunSetting Files to be processed';
echo 'Submitting Processing of raw data!'
index=0
for i in $*;
do
echo './process '$i;
./process $i;
done
echo 'Finished processing!'
else
echo 'ERROR: no RunSettingfiles provided!';
echo 'Syntax ' $0 ' RunSettingfilefilename (including Wildchars!)';

fi
 
