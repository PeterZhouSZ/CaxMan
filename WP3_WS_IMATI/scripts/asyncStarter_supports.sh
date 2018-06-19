#!/bin/bash

nohup /usr/local/bin/launchRemote_supports.sh $@ &> /tmp/$2/log.txt &
