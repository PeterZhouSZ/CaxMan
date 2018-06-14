#!/bin/bash

nohup /usr/local/bin/launchRemote_degeneratecleaner.sh $@ &> /tmp/$2/log.txt &
