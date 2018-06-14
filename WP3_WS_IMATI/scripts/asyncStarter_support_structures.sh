#!/bin/bash

nohup /usr/local/bin/launchRemote_support_structures.sh $@ &> /tmp/$2/log.txt &
