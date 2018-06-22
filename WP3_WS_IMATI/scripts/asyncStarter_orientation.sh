#!/bin/bash

nohup /usr/local/bin/launchRemote_orientation.sh $@ &> /tmp/$2/log_orientation.txt &
