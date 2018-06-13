#!/bin/env python
import suds
import urllib2
import sys
if len(sys.argv) < 4:
    print ("Usage:")
    print ("\t %s gss-url outputfilename token [local|test]" % sys.argv[0])
    exit()

# get url: 
url = sys.argv[1]
outputfileName = sys.argv[2]
sessionToken = sys.argv[3]

wsdlLocation = "https://api.eu-cloudflow.eu/sintef/gss-0.1/FileUtilities?wsdl"
if len(sys.argv) > 4:
    if sys.argv[4] == "local":
        wsdlLocation = "http://localhost:8080/gss/FileUtilities?wsdl"
    elif sys.argv[4] == "test":
        wsdlLocation = "https://api.eu-cloudflow.eu/sintef/gss_test/FileUtilities?wsdl"
        # Location for test versions of GSS

client = suds.client.Client(wsdlLocation)
#print client

resourceInformation = client.service.getResourceInformation(url, sessionToken)
#print resourceInformation

readDescription = resourceInformation.readDescription
print readDescription

if readDescription.supported:
    headers = {}
    headers[readDescription.sessionTokenField] = sessionToken
    if hasattr(readDescription, "headers"):
        for headerField in readDescription.headers:
            headers[headerField.key] = headerField.value

    with open(outputfileName, "wb") as outputFile:
        request = urllib2.Request(url = readDescription.url, headers=headers)           
        
        result = urllib2.urlopen(request)
        while True:
            buffer = result.read()
            if not buffer:
                break
            outputFile.write(buffer)
            #print buffer
else:
    print "The given gss_url does not support read/download."
