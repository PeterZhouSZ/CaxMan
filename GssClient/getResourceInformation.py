#!/bin/env python

#Copyright STIFTELSEN SINTEF 2016

import suds
import urllib2
import sys
if len(sys.argv) < 3:
    print ("Usage:")
    print ("\t %s gss-url token" % sys.argv[0])
    exit()

# get url: 
url = sys.argv[1]
sessionToken = sys.argv[2]

wsdlLocation = "https://caxman.clesgo.net/sintef/infrastructure/gss-0.1/FileUtilities?wsdl"

client = suds.client.Client(wsdlLocation)

resourceInformation = client.service.getResourceInformation(url, sessionToken)
print resourceInformation
