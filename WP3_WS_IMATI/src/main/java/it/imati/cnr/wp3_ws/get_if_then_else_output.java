/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.imati.cnr.wp3_ws;

import javax.jws.WebService;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.xml.ws.Holder;

/**
 *
 * @author daniela
 */
@WebService(serviceName = "get_if_then_else_output")
public class get_if_then_else_output {

    // The namespace should match the package name in the first non-commented line of this file. 
    // If package name is a.b.c, the namespace should be "http://c.b.a/" (casae sensitive)
    // WFM will have an easier time recognizing your web service if this is fulfilled
    //
    private final String namespace = "http://wp3_ws.cnr.imati.it/";
        
    
    @WebMethod(operationName = "get_output")
    public void get_output(
            @WebParam(name            = "serviceID", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String serviceID,
            
            @WebParam(name            = "sessionToken", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String sessionToken,
            
             @WebParam(name            = "if_then_else_output", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String if_then_else_output,
            
            @WebParam(name            = "output", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<String> output ) {
        
        output.value = if_then_else_output;
    }
}
