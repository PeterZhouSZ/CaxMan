#!/bin/bash

# Just make sure we get sane arguments:
if [ "$#" != 5 ];
then
    echo "Usage: $0 <sessionID> <statusFile> <fileToUpload> <resultFile> <gssOutputFolder>"
    exit
fi

sessionID=$1
statusFile=$2
fileToUpload=$3
resultFile=$4
gssOutputFolder=$5

# Do the heavy sleeping job and report status now and then
for i in `seq 1 99`
do
    if (($i % 7 == 0))
    then
	echo $i > $statusFile
    fi
    sleep 1s;
done


# Create a file that we will upload to the gssOutputFolder by using GSS

echo "This is the result from the async service. It should now be uploaded to a output folder in GSS" > $fileToUpload
echo "Time is:" >> $fileToUpload
date >> $fileToUpload
echo " " >> $fileToUpload 

gssCreatePath=$gssOutputFolder"output-`date +%F-%H%M%S`.txt"
echo $gssCreatePath
# Upload to GSS:
python /usr/local/bin/upload_gss.py $gssCreatePath $fileToUpload $sessionID > $resultFile


