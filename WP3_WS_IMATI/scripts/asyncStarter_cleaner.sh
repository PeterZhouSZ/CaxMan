#!/bin/bash

nohup /usr/local/bin/launchRemote_cleaner.sh $@ &> /tmp/$2/log_degcleaner.txt &
