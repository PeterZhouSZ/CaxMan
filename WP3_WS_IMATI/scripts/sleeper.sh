#!/bin/bash
 
# Just make sure we get sane arguments:
if [ "$#" != 2 ];
then
    echo "Usage: $0 <statusfile> <finalfile>"
    exit
fi
 
echo "Starting"
 
statusFile=$1 # First argument to bash script (0 is name of script)
finalFile=$2 # Second argument
 
for i in `seq 1 100` # Run a simple for loop for 100 iterations.
do
    echo "Some output to command line just to distract"
    # Important: we are overriding the content each time (intentionally)!
    echo $i > $statusFile;
    sleep 1s;
done
 
echo "Some important result that we care about and that should be uploaded to GSS eventually" > $2
date >> $2
