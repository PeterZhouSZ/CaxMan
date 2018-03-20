#!/usr/bin/env python
from xml.dom import minidom

# Used to count headings
headingCount = 0

checkableElements = ['radiobutton']

#Page Congfig Elements
pageConfig={}
pageLayout='left2right'
orientation='multiple'


import os
THIS_FOLDER = os.path.dirname(os.path.abspath(__file__))
my_file = os.path.join(THIS_FOLDER, 'pageConfig.xml')


def readConfigParam():	
	global pageLayout
	global orientation
	#xmldoc = minidom.parse('pageConfig.xml')
	xmldoc = minidom.parse(my_file)
	itemlist = xmldoc.getElementsByTagName('param')
	#Create dict
	for s in itemlist:
		pageConfig[str(s.attributes['name'].value)]=str(s.childNodes[0].nodeValue)
	pageLayout = pageConfig['layout']
	orientation = pageConfig['orientation']
	#print repr(pageConfig)	
	
def generate(getType, key, page):
#Returns the function name based on Type
	readConfigParam()
	
	#print key
	global checkableElements
	
	default_val = key['Default Value']
	
	#Filter Default value , ex. 'n/a' , '--', replaced by ''
	default_value = filter(default_val)
	
	
	min = key['Min Value']
	max  = key['Max Value']
	unitRaw = key['Unit of measurement']
	#Filter Unit of measurement, ex. 'n/a' , '--', replaced by ''
	unit = filter(unitRaw)
	
	name = key['Name']
	
	#print min+" "+max+" "+unit+" "+name+" "+default_value
	
	getFunction = switch.get(str(getType),'Not_Found')
	#if not found in the dataType list then generate a default text input field
	
	#Starting new Section
	if(not(getType in checkableElements)):
		page+=startNewSection()
	
	if getFunction == 'Not_Found':
		page+=formattingforNextElement()
		value = createDefaultStringField(default_value,min,max,name,unit)
		page+=value
		page+=endSection()
	else:
		page+=formattingforNextElement()
		if(getType in checkableElements and key['checked'] == 'YES'):
			value = getFunction(default_value,min,max,name,unit,True)
		else:
			value = getFunction(default_value,min,max,name,unit)
		page+=value
		page+=endSection()
	#if getFunction == 'Not_Found':
		#return False
	#print name
	
	#End Current Section
	if(not(getType in checkableElements)):
		page+=endSection()
	return page
	
#Filtering input util
def filter(inp):
	if(inp == 'n/a'):
		print 'unit of measurement is  '+str(inp)
		return ''
	elif(inp == '--'):
		print 'unit of measurement is  '+str(inp)
		return ''
	elif(inp == 'na'):
		print 'unit of measurement is  '+str(inp)
		return ''
	else:
		return inp
	
#Page Individual Elements Declared Below	
	
def createDecimalField(default_value,min,max,name,unit):
		print "Number Field"+name
		#return name+": <input type=\"number\" name=\""+name+"\"value=\""+default_value+"\"> "+unit+"</br>"
		if(unit == ''):
			return "<div class=\"col-xs-12\">\n\t<label for=\"usr\">"+name+"</label>\n</div>\n"+"<div class=\"col-xs-12\">\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\"  max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n</div>\n" + horizontalLine()
		else:
			return "<div class=\"col-xs-12\">\n\t<label for=\"usr\">"+name+"("+unit+")</label>\n</div>\n"+"<div class=\"col-xs-12\">\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\" max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n</div>\n" + horizontalLine()
			
			
def create3DecimalField(default_value,min,max,name,unit):
		print "Number Field"+name
		#return name+": <input type=\"number\" name=\""+name+"\"value=\""+default_value+"\"> "+unit+"</br>"
		if(unit == ''):
			return "<div class=\"col-xs-12\">\n\t<label for=\"usr\">"+name+"</label>\n</div>\n"+"<div class=\"col-xs-12\">\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\"  max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\"  max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\"  max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n</div>\n" + horizontalLine()
		else:
			return "<div class=\"col-xs-12\">\n\t<label for=\"usr\">"+name+"("+unit+")</label>\n</div>\n"+"<div class=\"col-xs-12\">\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\" max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\" max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\" max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n</div>\n" + horizontalLine()

def create6DecimalField(default_value,min,max,name,unit):
		print "Number Field"+name
		#return name+": <input type=\"number\" name=\""+name+"\"value=\""+default_value+"\"> "+unit+"</br>"
		if(unit == ''):
			return "<div class=\"col-xs-12\">\n\t<label for=\"usr\">"+name+"</label>\n</div>\n"+"<div class=\"col-xs-12\">\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\"  max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\"  max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\"  max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/><input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\"  max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\"  max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\"  max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n</div>\n" + horizontalLine()
		else:
			return "<div class=\"col-xs-12\">\n\t<label for=\"usr\">"+name+"("+unit+")</label>\n</div>\n"+"<div class=\"col-xs-12\">\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\" max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\" max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\" max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\" max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\" max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n\t<input id =\"param_Name\" type=\"number\" name=\""+name+"\" min =\""+min+"\" max =\""+max+"\"class=\"form-control\" value=\""+default_value+"\"/>\n</div>\n" + horizontalLine()


def createStringField(default_value,min,max,name,unit):
		print "String Field"+name
		#return name+": <input type=\"text\" name=\""+name+"\"value=\""+default_value+"\"> "+unit+"</br>"
		if(unit == ''):
			return "<div class=\"col-xs-12\">\n\t<label for=\"usr\">"+name+"</label>\n</div>\n"+"<div class=\"col-xs-12\">\n\t<input id =\"param_Name\" type=\"text\" name=\""+name+"\" class=\"form-control\" value=\""+default_value+"\"/>\n</div>\n" + horizontalLine()
		else:
			return "<div class=\"col-xs-12\">\n\t<label for=\"usr\">"+name+"("+unit+")</label>\n</div>\n"+"<div class=\"col-xs-12\">\n\t<input id =\"param_Name\" type=\"text\" name=\""+name+"\" class=\"form-control\" value=\""+default_value+"\"/>\n</div>\n" + horizontalLine()
		
def createRangeField(default_value,min,max,name,unit):
		return True
		
def createDefaultStringField(default_value,min,max,name,unit):
		print "Default Field"+name
		if(unit == ''):
			return "<div class=\"col-xs-12\">\n\t<label for=\"usr\">"+name+"</label>\n</div>\n"+"<div class=\"col-xs-12\">\n\t<input id =\"param_Name\" type=\"text\" name=\""+name+"\" class=\"form-control\" value=\""+default_value+"\"/>\n</div>\n" + horizontalLine()
		else:
			return "<div class=\"col-xs-12\">\n\t<label for=\"usr\">"+name+"("+unit+")</label>\n</div>\n"+"<div class=\"col-xs-12\">\n\t<input id =\"param_Name\" type=\"text\" name=\""+name+"\" class=\"form-control\" value=\""+default_value+"\"/>\n</div>\n" + horizontalLine()
		
def startNewSection():
	return "<div class=\"col-xs-4\">\n"
	
def endSection():
	return "</div>\n"
	
def formattingforNextElement():
	return "<div class=\"row\">\n"
	
def createRadioButton(default_value,min,max,name,unit,checked=False):
	print "radiobutton " + name
	
	if(checked):
		return "<div class=\"col-xs-12\">\n\t<label><input class=\""+unit+"_radiobuttons\" type=\"radio\" checked=\"checked\" name=\"optradio_"+unit+"\" value=\""+default_value+"\">"+name+"</label>\n</div>\n"
	else:
		return "<div class=\"col-xs-12\">\n\t<label><input class=\""+unit+"_radiobuttons\" type=\"radio\" name=\"optradio_"+unit+"\" value=\""+default_value+"\">"+name+"</label>\n</div>\n"
	
def createHeading(default_value,min,max,name,unit):
	global headingCount
	print "Heading " + name
	
	ret = "</div><p><div class=\"col-xs-12\" style='display: inline-block;'><h4><b>"+name+"</b></h4></div></p>\n"
	if headingCount != 0:
		ret = horizontalLine() + ret
	headingCount += 1
	return ret
	
def horizontalLine():
	return "<hr/>\n"
	
#Dictionary for the dataTypes
switch = {'decimal': createDecimalField, 'string': createStringField,'range': createRangeField,'3 decimals':create3DecimalField,'6 decimals':create6DecimalField, 'radiobutton':createRadioButton, 'heading':createHeading}

