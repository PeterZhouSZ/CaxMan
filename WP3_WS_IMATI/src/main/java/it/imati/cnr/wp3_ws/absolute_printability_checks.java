/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.imati.cnr.wp3_ws;

import java.io.File;
import java.io.IOException;
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
@WebService(serviceName = "absolute_printability_checks")
public class absolute_printability_checks 
{
 
    // The namespace should match the package name in the first non-commented line of this file. 
    // If package name is a.b.c, the namespace should be "http://c.b.a/" (casae sensitive)
    // WFM will have an easier time recognizing your web service if this is fulfilled
    //
    private final String namespace = "http://wp3_ws.cnr.imati.it/";
        
    
    @WebMethod(operationName = "absolute_printability_checks_opname")
    public void absolute_printability_checks_operation (
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
        try
        {
            String pathGSSTools         = "/root/infrastructureClients/gssClients/gssPythonClients/";
            String pathOrientationTool  = "/root/CaxMan/detect_voids_service/";
            String downloadedFilename   = "/root/dowloaded.off";      
            String checkedFilename      = "/root/checked.ann";
            String outputURI            = "swift://caxman/imati-ge/checked.ann";
            
            // Download File
            String cmdDownload = "python " + pathGSSTools + "download_gss.py " + annotated_STL_URI_in + " " + downloadedFilename + " " + sessionToken;
            Process p1 = Runtime.getRuntime().exec(cmdDownload);
            
            // Run orientation
            String cmdRunDetectVoids = pathOrientationTool + "detect_voids_service " + downloadedFilename + " " + checkedFilename;
            Process p2 = Runtime.getRuntime().exec(cmdRunDetectVoids);
            
            // Check output existence
            File f = new File(checkedFilename);
            if(!f.exists() || f.isDirectory()) throw new IOException("File does not exist");
          
            // Upload output
            String cmdUploadOutput = "python " + pathGSSTools + "upload_gss.py " + outputURI + " " + checkedFilename + " " + sessionToken;
            Process p3 = Runtime.getRuntime().exec(cmdUploadOutput);
               
            annotated_STL_URI_out.value      = outputURI;
            absolute_printability_flag.value = true;
            
        }
        catch(IOException e)
        {           
            annotated_STL_URI_out.value      = "";
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
        Logger.getLogger(absolute_printability_checks.class.getName()).log(Level.SEVERE, null, ex);
    }}
