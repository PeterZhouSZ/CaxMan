#!/bin/bash

TOKEN_GSS=$1
INPUT_MODEL_GSS=$2
OUTPUT_VOLUME_GSS=$3
OUTPUT_VOLUME_GSS=$4

HATCH_THICKNESS=$5

TARGET_DIR=$6
STATUS_FILE=$7
RESULT_FILE_VOLUME=$8
RESULT_FILE_SURFACE=$9

OUTPUT_FILE_VOLUME=${10}
OUTPUT_FILE_SURFACE=${11}

DOWNLOAD_TOOL="/root/infrastructureClients/gssClients/gssPythonClients/download_gss.py"
UPLOAD_TOOL="/root/infrastructureClients/gssClients/gssPythonClients/upload_gss.py"

SLICE2MESH_TOOL="/root/CaxMan/demo_services/slice_2_mesh/build/slice2mesh"

echo "20" > $TARGET_DIR/$STATUS_FILE

DOWNLOADED_FILE=$TARGET_DIR/input.cli

rm -f $DOWNLOADED_FILE
rm -f $OUTPUT_FILE_VOLUME
rm -f $OUTPUT_FILE_SURFACE

# download file from gss
cmd_download="python $DOWNLOAD_TOOL $INPUT_MODEL_GSS $DOWNLOADED_FILE $TOKEN_GSS"

echo $cmd_download

$cmd_download


echo "40" > $TARGET_DIR/$STATUS_FILE

# run orientation optimization
$SLICE2MESH_TOOL $DOWNLOADED_FILE $OUTPUT_FILE_SURFACE $OUTPUT_FILE_VOLUME $HATCH_THICKNESS

echo "70" > $TARGET_DIR/$STATUS_FILE

# upload output volume to gss
cmd_upload_volume="python $UPLOAD_TOOL $OUTPUT_VOLUME_GSS $OUTPUT_FILE_VOLUME $TOKEN_GSS"

echo $cmd_upload_volume

echo "80" > $TARGET_DIR/$STATUS_FILE

$cmd_upload_volume > $RESULT_FILE_VOLUME


# upload output surface to gss
cmd_upload_surface="python $UPLOAD_TOOL $OUTPUT_SURFACE_GSS $OUTPUT_FILE_SURFACE $TOKEN_GSS"

echo $cmd_upload_surface

echo "90" > $TARGET_DIR/$STATUS_FILE

$cmd_upload_surface > $RESULT_FILE_SURFACE
