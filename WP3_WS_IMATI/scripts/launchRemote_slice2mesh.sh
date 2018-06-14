#!/bin/bash
#
#
session_id=$1
service_id=$2
status_file=$3
result_volume=$4
result_surface=$5
volume_to_upload=$6
surface_to_upload=$7
gss_output_folder=$8

#start: added for orientation optimization
mesh_in=$9
hatch_thickness=${10}
out_volume_uri=${11}
out_surface_uri=${12}
#end

logFile=/tmp/training_log.txt

echo -e "\n\nAsync service deployed on 10.30.1.106" >> $logFile
echo "service_id: $service_id" >> $logFile
echo "status_file: $status_file" >> $logFile

status_file_basename=$(basename "$status_file")
result_file_basename=$(basename "$result_file")
file_to_upload_basename=$(basename "$file_to_upload")
echo "status_file_basename: $status_file_basename" >> $logFile

target_dir=/tmp/$service_id
echo "target_dir: $target_dir"

# Create the target dir on the local and remote machine.
#mkdir -p $target_dir <-- done from java from before
#ssh root@10.30.1.112 mkdir -p $target_dir
mkdir -p $target_dir

# Make sure there is a status file:
echo "0" > $target_dir/$status_file_basename

# Start the long running job on the compute machine
echo "Starting the remote job ..."  >> $logFile
#ssh root@10.30.1.112 "/usr/local/bin/asyncSleeper.sh $session_id $target_dir/$status_file_basename $target_dir/$file_to_upload_basename $target_dir/$result_file_basename $gss_output_folder"

cmd="/usr/local/bin/slice2mesh.sh $session_id $mesh_in $out_volume_uri $out_surface_uri $hatch_thickness $target_dir $result_volume $result_surface $volume_to_upload $surface_to_upload"

echo "RUNNING $cmd"

$cmd

echo "The remote job is done."  >> $logFile

# Fetch the data.
#scp root@10.30.1.112:$target_dir/$result_file_basename $target_dir/$result_file_basename

# Print status to remote status file (which is the one being read by getServiceStatus
#ssh root@10.30.1.112 "echo 100 > $target_dir/$status_file_basename"
echo 100 > $target_dir/$status_file_basename
