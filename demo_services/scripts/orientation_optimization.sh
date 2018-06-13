#!/bin/bash

TOKEN_GSS=$1
INPUT_MODEL_GSS=$2
OUTPUT_MODEL_GSS=$3

WS=$4
WT=$5
WQ=$6

THRESHOLD=$7
NDIRS=$8

DOWNLOAD_TOOL="/root/infrastructureClients/gssClients/gssPythonClients/download_gss.py"
UPLOAD_TOOL="/root/infrastructureClients/gssClients/gssPythonClients/upload_gss.py"

ORIENTATION_TOOL="/root/CaxMan/demo_services/orientation_optimization/build/orient_service"

DOWNLOADED_FILE=/root/CAxManIO/input.off
OUTPUT_FILE=/root/CAxManIO/output.off

rm -f $DOWNLOADED_FILE
rm -f $OUTPUT_FILE

# download file from gss
cmd_download="python $DOWNLOAD_TOOL $INPUT_MODEL_GSS $DOWNLOADED_FILE $TOKEN_GSS"

echo $cmd_download

$cmd_download

# run orientation optimization 
$ORIENTATION_TOOL $DOWNLOADED_FILE $OUTPUT_FILE $WQ $WT $WS $THRESHOLD $NDIRS

# upload outputt file to gss 
cmd_upload="python $UPLOAD_TOOL $OUTPUT_MODEL_GSS $OUTPUT_FILE $TOKEN_GSS"

echo $cmd_upload

$cmd_upload
