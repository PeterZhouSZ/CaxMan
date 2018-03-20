#!/usr/bin/env python
import logging
#logging.basicConfig(filename='log.out',level=logging.INFO)
import pysimplesoap
from pysimplesoap.server import SoapDispatcher, SOAPHandler
from pysimplesoap.client import SoapClient
import httplib, urllib
from BaseHTTPServer import HTTPServer
import base64
from subprocess import Popen
from thread import start_new_thread
import time
import sys
import uuid
import re
from readFromXls import xls2JSON

import json
import re

#logger.setLevel(logging.INFO)


sessions = {}
instances = 0

import os
import signal
import requests

#Changes for using page generator in PythonTools Feb 6, 2018
additionalPath = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', '../PythonTools/PageGenerator/'))
sys.path.append(additionalPath)
#additionalPath2 = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', '../PythonTools/'))
#sys.path.append(additionalPath2)





from generator import generate

servID = ''
sessToken = ''
wsInputs = {}
nameValPair = {}

def startApplication(serviceID, sessionToken):
	#print ' serviceID: '+serviceID+' sessionToken: '+sessionToken
	#print 'In start Application'

	global servID
	servID = serviceID
	global sessToken
	sessToken = sessionToken

	#print servID
	#print sessToken


	#Load Module using to extract and parse
	readPageHeader = returnFileAsString('genericPageHeader.dat')

	#Generate Page Body

	returnobj = {}
	readPageData = returnFileAsString('genericPageBody.dat')

	readPageBottom = returnFileAsString('genericPageBottom.dat')

	addFieldValues = '<input type="hidden"  id="FieldValues" value="''" />'
	read_data = readPageHeader+readPageData+addFieldValues+readPageBottom
	outData = encodeDatab64(read_data)


	#Appending the encoded page to the object
	#returnobj = nameValPair
	returnobj = {}
	returnobj['status_base64'] = outData
	returnobj.update(nameValPair)
	#print '---------------------------'
	#print returnobj
	#print '---------------------------'

	return returnobj



	#Alternate approach could be using pysimplesoap

	#client = SoapClient(wsdl='https://api.caxman.eu/dfki/tomcat/WorkflowManager2Service/services/SOAP?wsdl', trace=True)
	#client.serviceExecutionFinishe({'serviceID':servID,
    #                                 'sessionToken':sessToken,
    #                                 'xmlOutputs_base64': xmlOutputs_base64})


	return

def returnFileAsString(filename):
	with open(filename,'r') as f:
		data = f.read()
	return data

#Base64 encoding function
def encodeDatab64(dataInp):
	b64encoded_data = base64.b64encode(dataInp)
	return b64encoded_data

def performTypeCheck(inpType):
	if("decimal" in str(inpType)):
		return float
	else:
		return str

def formatName(tempName):
	##val = re.sub('[^A-Za-z0-9]+', '', tempName)
	val = re.sub('[^A-Za-z]+', '_', tempName)
	return val

#Genration of Page Body
def getPage():
	getJSONOBj = xls2JSON()
	page=""
	makeDict = json.loads(getJSONOBj)
	#print makeDict
	#print type(makeDict)
	success = True
	status_return={}
	for key in makeDict:
		#for labels in key:

		#Checking type for generating corresponding HTML field

		getType = key['Type']
		#Generating Dict Object
		default_value = key['Default Value']
		tempName = key['Name']
		#Format the wsdl field generated here
		name = formatName(tempName)
		#status_return[str(name)] = str(default_value)
		status_return[str(name)] = performTypeCheck(getType)
		#Creating simple String for Display as result
		global nameValPair
		if(performTypeCheck(getType) == str):
			nameValPair[str(name)] = str(default_value)
			##nameValPair[str(name)] = str(default_value)
		elif(performTypeCheck(getType) == float):
			if(type(default_value)==str):
				nameValPair[str(name)] = str(default_value)
			else:
				nameValPair[str(name)] = str(default_value)
		else:
			nameValPair[str(name)] = str(default_value)

		page = generate(getType, key, page)
		if not success :
			break


	#Remove the existing page and create new
	filename = 'genericPageBody.dat'
	try:
	#Removing existing file
		os.remove(filename)
		print('File Removed Successfully')
	except OSError:
		pass

	with open('genericPageBody.dat', 'a') as f:
		f.write(page)
		#print getval+'\n'
	return status_return
	#return nameValPair

#Resolving a CORS issue for response for latest Firefox update
# Info : https://developer.mozilla.org/en-US/docs/Web/HTTP/Access_control_CORS
class MySOAPHandler(SOAPHandler):
    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header("Access-Control-Allow-Headers", "X-Requested-With")
        self.end_headers()

def startApp(location='https://api.caxman.eu/igd/examples/WebApplication', action='https://api.caxman.eu/igd/examples/WebApplication'):

	dispatcher = SoapDispatcher(
			'ApplicationInterface',
			location = location,
			action = action, # SOAPAction
			namespace = "http://example.com/sample.wsdl", prefix="ns0",
			trace = True,
			ns = True)

	dispatcher.register_function('startApplication', startApplication,
			#adding all output values as parameters with their Datatype
			returns=wsInputs,
			args={'serviceID':str, 'sessionToken':str})

	#dispatcher.register_function('serviceExecutionFinishCall', servcExecFinishCall,
			#returns={},
			#args={'xmlOutputs':str})


	logging.info('Starting App Interface...')
	print 'Starting Application Interface...'
	httpd = HTTPServer(("", 8000), MySOAPHandler)
	print 'Server started...'
	logging.info('constructed')
	httpd.dispatcher = dispatcher
	logging.info('start loop...')
	httpd.serve_forever()

if __name__ == '__main__':
	#Generating the return type

	#getDefaultValues = str(getPage())
	global wsInputs
	wsInputs['status_base64'] = str
	wsInputs.update(getPage())
	#print wsInputs
	#print type(getDefaultValues)
	startApp()
