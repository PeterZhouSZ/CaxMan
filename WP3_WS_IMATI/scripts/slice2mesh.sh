#!/bin/bash

TOKEN_GSS=$1
INPUT_MODEL_GSS=$2
OUTPUT_VOLUME_GSS=$3
OUTPUT_SURFACE_GSS=$4

HATCH_THICKNESS=$5

TARGET_DIR=$6
RESULT_VOLUME=$7
RESULT_SURFACE=$8

OUTPUT_VOLUME=$9
OUTPUT_SURFACE=${10}

DOWNLOAD_TOOL="/root/infrastructureClients/gssClients/gssPythonClients/download_gss.py"
UPLOAD_TOOL="/root/infrastructureClients/gssClients/gssPythonClients/upload_gss.py"

SLICE2MESH_TOOL="/root/CaxMan/demo_services/slice_2_mesh/build/slice2mesh"

DOWNLOADED_FILE=$TARGET_DIR/input.cli

rm -f $DOWNLOADED_FILE
rm -f $OUTPUT_VOLUME
rm -f $OUTPUT_SURFACE

# download file from gss
cmd_download="python $DOWNLOAD_TOOL $INPUT_MODEL_GSS $DOWNLOADED_FILE $TOKEN_GSS"

echo $cmd_download

$cmd_download

# run orientation optimization
$SLICE2MESH_TOOL $DOWNLOADED_FILE $OUTPUT_SURFACE $OUTPUT_VOLUME $HATCH_THICKNESS

# upload outputt file to gss
cmd_upload_volume="python $UPLOAD_TOOL $OUTPUT_VOLUME_GSS $OUTPUT_VOLUME $TOKEN_GSS"

echo $cmd_upload_volume

$cmd_upload_volume > $RESULT_VOLUME


cmd_upload_surface="python $UPLOAD_TOOL $OUTPUT_SURFACE_GSS $OUTPUT_SURFACE $TOKEN_GSS"

echo $cmd_upload_surface

$cmd_upload_surface > $RESULT_VOLUME
