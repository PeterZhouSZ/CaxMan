#!/bin/bash

nohup /usr/local/bin/launchRemote_slicer.sh $@ &> /tmp/$2/log.txt &
