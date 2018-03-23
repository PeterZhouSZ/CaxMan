/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.imati.cnr.wp3_ws;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.annotation.Resource;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import javax.xml.bind.DatatypeConverter;
import javax.xml.ws.Holder;
import javax.xml.ws.WebServiceContext;


/**
 *
 * @author havahol
 */


// TODO: Make a pretty name for your web service endpoint
@WebService(serviceName = "pp_gui")
public class gui {

    // TODO:
    // The namespace should match the package name in line 1. 
    // If package name is a.b.c, the namespace should be "http://c.b.a/" (casae sensitive)
    // WFM will have an easier time recognizing your web service if this is fulfilled
    private final String namespace = "http://wp3_ws.cnr.imati.it/";
    
    @Resource
    private WebServiceContext context;

    
    // TODO: Make a pretty name for the web service
    @WebMethod(operationName = "webApplication")
    public void webApplication(@WebParam(name = "serviceID", targetNamespace=namespace, mode=WebParam.Mode.IN) String serviceID, 
            @WebParam(name = "sessionToken", targetNamespace=namespace, mode=WebParam.Mode.IN) String sessionToken,
            @WebParam(name = "extraParameters", targetNamespace=namespace, mode=WebParam.Mode.IN) String extraParameters,
            @WebParam(name="url", targetNamespace=namespace, mode=WebParam.Mode.IN) String url,
            @WebParam(name="status_base64", targetNamespace=namespace, mode=WebParam.Mode.OUT) Holder<String> status_base64
            ) {

        // serviceID, sessionToken and status_base64 is manditory input to a Cloudflow Application.
        
        // TODO: Feel free to add more input parameters   
        // TODO: Feel free to add more output parameters (they must be Holder<T> objects
        
        // TODO: url should point to the web pages you want to include (should be available under "https://api.eu-cloudflow.eu/[...]"
        url = "https://caxman.clesgo.net/imati/wp3/parameters.html";
        
        
        // Obtaining the SOAP endpoint for WFM so that we are able to let WFM know when
        // the web application is finished.
        // Depending of your client you will need that the endpoint includes "?wsdl". 
        // You will have to check this for your own SOAP client, but our javascript client requires
        // "?wsdl" to not be a part of the endpoint.
        String WFMsoap = getExtraParameter(extraParameters, "WFM");
        WFMsoap = WFMsoap.replace("?wsdl", "");
        
        String containingHtml = "<html><head><title>Distance viewer</title>\n<style>html, body  {\n" +
"                height: 100%;\n" +
"                overflow: hidden;\n" +
"            }\n" +
"            html, body, iframe {\n" +
"                margin: 0;\n" +
"                padding: 0;\n" +
"            }\n</style>"
                +"<script type='text/javascript'>"
                // Set the variables so that the HTML page in the iframe will have access to them through parent
                // TODO: add your extra input parameters here javascript variables here
                + "sessionToken = \"" + sessionToken + "\";\n"
                + "serviceID = \"" + serviceID + "\";\n"
                + "WFMsoap = \"" + WFMsoap + "\";\n"
                + "hasCloudflowVariables = true;\n"
                + "</script>\n"
                + "</head>\n"
                + "<body>\n"
                +"<iframe src='" + url + "' width='100%' height='100%' seamless='1' style='height: 100%; width: 100%;border-style: none; overflow: hidden;'>\n"
                +"</body>\n"
                + "</html>";
        
        status_base64.value = DatatypeConverter.printBase64Binary(containingHtml.getBytes());
        
        // TODO: If your web application service contains output parameters other than "status_base64", you should make sure they are not null
        // when this method returns. This value will be ignored by WFM, since WFM will expect the proper output values when you call WFM's "serviceExecutionFinish" service
        // outputValue1.value = "UNSET"; // if outputValue1 is String
        // outputValue2.value = 0;  // if outputValue2 is int/float/double
        
        
        // TODO: Check ProxyFilter.java line 153 ;)
    }


    // Function for parsing the extraParameters input 
    private String getExtraParameter(String extraParameters, String key) {
        String[] map = extraParameters.split(",");
        for (String pair : map) {
            String[] keyValue = pair.split("=");
            if (keyValue.length == 2 ) {
                //log("::getExtraParameter - found {" + keyValue[0] + ", " + keyValue[1] + "}");
                if (keyValue[0].equals(key)) {
                    return keyValue[1];
                }
            }
            else {
                String wierd = "Something is wrong with the extraParameters! Found key-value pair of length: " + keyValue.length + "\n{";
                for (String str : keyValue) {
                    wierd += keyValue + str + ", ";
                }
                log(wierd);
            }
        }
        return "notFound";
    }    
    
    // Logging into the GlassFish log 
    // 
    private void log(String message) {
        Logger.getLogger(this.getClass().toString()).log(Level.INFO, this.getClass().getSimpleName() + "::" + message);
    }
    
    
}
