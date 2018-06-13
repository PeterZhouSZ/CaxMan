#!/bin/env python
import suds
import urllib2
import sys
import os
if len(sys.argv) < 4:
    print ("Usage:")
    print ("\t %s gss-url inputfilename token [test]" % sys.argv[0])
    exit()

# get url: 
url = sys.argv[1]
filenameIn = sys.argv[2]
sessionToken = sys.argv[3]
wsdl_url = "https://api.eu-cloudflow.eu/sintef/gss-0.1/FileUtilities?wsdl"

if len(sys.argv) > 4:
    if (sys.argv[4] == "test"):
        wsdl_url = "https://api.eu-cloudflow.eu/sintef/gss_test/FileUtilities?wsdl"
        # GSS test location

client = suds.client.Client(wsdl_url)

resourceInformation = client.service.getResourceInformation(url, sessionToken)
createDescription = resourceInformation.createDescription

if createDescription.supported:
    headers = {}
    headers[createDescription.sessionTokenField] = sessionToken
    headers["Content-Length"]= "%d" % os.stat(filenameIn).st_size
    if hasattr(createDescription, "headers"):
        for headerField in createDescription.headers:
            headers[headerField.key] = headerField.value
    
    with open(filenameIn, "r") as inputFile:
        tmpUrl = createDescription.url
        request = urllib2.Request(url = tmpUrl, data=inputFile, headers=headers)                 
        request.get_method = lambda: createDescription.httpMethod;
        result = urllib2.urlopen(request)
        #print "HTML response: " + str(result.getcode())
        
    if resourceInformation.queryForName:
        #print "\n"
        print result.info()["filename"]
    else:
        print url
else:
    print "Create is not a supported operation for this file"
