#!/bin/bash

nohup /usr/local/bin/launchRemote_slice2mesh.sh $@ &> /tmp/$2/log_slice2mesh.txt &
