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
@WebService(serviceName = "send_to_WP2")
public class send_to_WP2 {
  
    // The namespace should match the package name in the first non-commented line of this file. 
    // If package name is a.b.c, the namespace should be "http://c.b.a/" (casae sensitive)
    // WFM will have an easier time recognizing your web service if this is fulfilled
    //
    private final String namespace = "http://wp3_ws.cnr.imati.it/";
        
    
    @WebMethod(operationName = "send_to_WP2_opname")
    public void send_to_WP2(
            @WebParam(name            = "serviceID", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String serviceID,
            
            @WebParam(name            = "sessionToken", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String sessionToken,
            
            @WebParam(name            = "annotated_tessellation", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String annotated_tessellation,
            
            @WebParam(name            = "annotated_STL_URI_out", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<String> message )  {
        
        
        message.value = " A revision request has been sent to WP2. " ;
    }
}
