#!/usr/bin/env python

from readFromXls import xls2JSON

import json
import os
import sys

additionalPath = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', '../PythonTools/PageGenerator/'))
sys.path.append(additionalPath)


from generator import generate

getJSONOBj = xls2JSON()
page=""
makeDict = json.loads(getJSONOBj)
#print makeDict
#print type(makeDict)
success = True
for key in makeDict:
	#for labels in key:
	
#Checking type for generating corresponding HTML field
		
	getType = key['Type']
	page = generate(getType, key, page)
	if not success :
		break
		#print labels
		#print type(labels)
		#getval = labels+'-------corresponds to-----'+key[labels]

#print "Generated"+page		
#Used for Debugging 		
with open('genericPageBody.dat', 'a') as f:
	f.write(page)
		#print getval+'\n'
		
		
#Test Page for Checking HTML output

def returnFileAsString(filename):
	with open(filename,'r') as f:
		data = f.read()
	return data



readPageHeader = returnFileAsString('genericPageHeader.dat')
	
#Generate Page Body
	
readPageData = returnFileAsString('genericPageBody.dat')

readPageBottom = returnFileAsString('genericPageBottom.dat')
	
addFieldValues = '<input type="hidden"  id="FieldValues" value="''" />'
read_data = readPageHeader+readPageData+addFieldValues+readPageBottom
		

with open('test.html', 'w') as f:
	f.write(read_data)
		
	