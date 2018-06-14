#!/bin/bash

nohup /usr/local/bin/launchRemote.sh $@ &> /tmp/$2/log.txt &

