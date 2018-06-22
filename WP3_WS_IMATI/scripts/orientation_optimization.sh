#!/bin/bash

TOKEN_GSS=$1
INPUT_MODEL_GSS=$2
OUTPUT_MODEL_GSS=$3

WS=$4
WT=$5
WQ=$6

THRESHOLD=$7
NDIRS=$8

TARGET_DIR=$9
STATUS_FILE=${10}
RESULT_FILE=${11}

OUTPUT_FILE=${12}

DOWNLOAD_TOOL="/root/infrastructureClients/gssClients/gssPythonClients/download_gss.py"
UPLOAD_TOOL="/root/infrastructureClients/gssClients/gssPythonClients/upload_gss.py"

ORIENTATION_TOOL="/root/CaxMan/demo_services/orientation_optimization/build/orient_service"

echo "20" > $TARGET_DIR/$STATUS_FILE

DOWNLOADED_FILE=$TARGET_DIR/input.off

rm -f $DOWNLOADED_FILE
rm -f $OUTPUT_FILE

# download file from gss
cmd_download="python $DOWNLOAD_TOOL $INPUT_MODEL_GSS $DOWNLOADED_FILE $TOKEN_GSS"

echo $cmd_download

$cmd_download


echo "40" > $TARGET_DIR/$STATUS_FILE

# run orientation optimization
cmd="$ORIENTATION_TOOL $DOWNLOADED_FILE $OUTPUT_FILE $WQ $WT $WS $THRESHOLD $NDIRS"

echo $cmd

$cmd
echo "70" > $TARGET_DIR/$STATUS_FILE

# upload outputt file to gss
cmd_upload="python $UPLOAD_TOOL $OUTPUT_MODEL_GSS $OUTPUT_FILE $TOKEN_GSS"

echo $cmd_upload

echo "90" > $TARGET_DIR/$STATUS_FILE

$cmd_upload > $RESULT_FILE
