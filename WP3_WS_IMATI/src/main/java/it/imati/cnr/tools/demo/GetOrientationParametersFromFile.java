/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.imati.cnr.tools.demo;

import javax.jws.WebService;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.xml.ws.Holder;

/**
 *
 * @author daniela
 */
@WebService(serviceName = "GetOrientationParametersFromFile")
public class GetOrientationParametersFromFile 
{

    private final String namespace = "http://demo.tools.cnr.imati.it/";

    /**
     * This is a sample web service operation
     * @param serviceID
     * @param sessionToken
     * @param param_file_in
     * @param wq
     * @param wt
     * @param ws
     * @param threshold
     * @param ndirs
     */
    @WebMethod(operationName = "GetOrientationParametersFromFileMethod")
    public void get_orientation_parameters(@WebParam(name            = "serviceID", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String serviceID,
            
            @WebParam(name            = "sessionToken", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String sessionToken,
            
            @WebParam(name            = "param_file_in",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  String param_file_in,
            
             @WebParam(name            = "wq",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.OUT) Holder<Double> wq,
             
              @WebParam(name            = "wt",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.OUT)  Holder<Double> wt,
              
               @WebParam(name            = "ws",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.OUT)  Holder<Double> ws,
               
                @WebParam(name            = "threshold",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.OUT)  Holder<Double> threshold ,
                
                 @WebParam(name            = "ndirs",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.OUT)  Holder<Integer> ndirs) 
    {
        
        
        
    }
}
