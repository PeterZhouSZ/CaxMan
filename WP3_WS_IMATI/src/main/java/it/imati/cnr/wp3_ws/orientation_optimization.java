/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.imati.cnr.wp3_ws;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.jws.WebService;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.xml.ws.Holder;

/**
 *
 * @author cino
 */
@WebService(serviceName = "orientation_optimization")
public class orientation_optimization 
{
 
    // The namespace should match the package name in the first non-commented line of this file. 
    // If package name is a.b.c, the namespace should be "http://c.b.a/" (casae sensitive)
    // WFM will have an easier time recognizing your web service if this is fulfilled
    //
    private final String namespace = "http://wp3_ws.cnr.imati.it/";
        
    
    @WebMethod(operationName = "orientation_optimization_opname")
    public void orientation_optimization_operation (
            @WebParam(name            = "serviceID", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String serviceID,
            
            @WebParam(name            = "sessionToken", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String sessionToken,
            
            @WebParam(name            = "annotated_STL_URI_in",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  String annotated_STL_URI_in,
            
            @WebParam(name            = "annotated_STL_URI_out", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<String> annotated_STL_URI_out,
            
            @WebParam(name            = "absolute_printability_flag", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<Boolean> absolute_printability_flag) 
    {        
        
        String line = "token vuoto";
        StringBuilder sb = new StringBuilder();
        
        try
        {
            String  path = "/root/infrastructureClients/gssClients/gssPythonClients/";
            String  cmd  = "python " + path + "getToken.py imati s8Q48TxUw=5 caxman";           
            Process p    = Runtime.getRuntime().exec(cmd);
            
            BufferedReader reader = new BufferedReader (new InputStreamReader(p.getInputStream()));
            
            line = reader.readLine();
            
            if (line != null) sb.append(line);
            
            String  token    = sb.toString();                       
            String  file_in  = "swift://caxman/imati-ge/T_supported.off";
            String  file_out = "/root/T_supported_oriented.off";
            
            String  cmd2     = "python " + path + "download_gss.py " + 
                               file_in + " " + file_out + " " + token;
            
            Process p2       = Runtime.getRuntime().exec(cmd2);
            
           
            String  file_up  = "swift://caxman/imati-ge/T_supported_oriented.off";
            String  cmd3     = "python " + path + "upload_gss.py " + file_up + " " +
                               file_out + " " + token;
            Process p3       = Runtime.getRuntime().exec(cmd3);
                       
            annotated_STL_URI_out.value      = cmd3;
            absolute_printability_flag.value = true;
            
        }
        catch(IOException e)
        {
            line = "CRASHED";
            
            annotated_STL_URI_out.value      = line;
            absolute_printability_flag.value = false;
        }                
    }
    
    
    /*
    *  Utility function for less verbose logging
    */
    private void log(String message) {
        Logger.getLogger(this.getClass().getName()).log(Level.INFO, message);
    }
    
    /*
    *  Utility function for less verbose error message in log
    */
    private void error(String message) {
        Logger.getLogger(this.getClass().getName()).log(Level.SEVERE, message);
    }
    
    /*
    *  Utility function for less verbose error message in log
    */
    private void error(IOException ex) {
        Logger.getLogger(orientation_optimization.class.getName()).log(Level.SEVERE, null, ex);
    }}
