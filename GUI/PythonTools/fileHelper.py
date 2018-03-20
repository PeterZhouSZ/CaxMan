#!/usr/bin/env python
#requirements
# pip install xmltodict
import xmltodict
import os.path

def parseXML(file):
	result = {}
	if(len(file)==0):
		return result
	if not os.path.isfile(file):
		return result
	with open(file, 'r') as fd:
		result = xmltodict.parse(fd.read())
	return result
