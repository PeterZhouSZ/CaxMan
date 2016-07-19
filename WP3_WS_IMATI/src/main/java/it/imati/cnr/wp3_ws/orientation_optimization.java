/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.imati.cnr.wp3_ws;

import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
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
        try
        {
            DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
            String sdate = dateFormat.format(new Date());
            
            String pathGSSTools         = "/root/infrastructureClients/gssClients/gssPythonClients/";
            String pathOrientationTool  = "/root/CaxMan/orientation_service/";
            String downloadedFilename   = "/root/CAxManIO/dowloaded_" + sdate + ".off";      
            String orientedFilename     = "/root/CAxManIO/oriented_" + sdate + ".ann";
            String outputURI            = "swift://caxman/imati-ge/oriented_" + sdate + ".ann";
            
            // Download File
            String cmdDownload = "python " + pathGSSTools + "download_gss.py " + annotated_STL_URI_in + " " + downloadedFilename + " " + sessionToken;
            
            System.out.print("[RUNNING] : " + cmdDownload);
            
            Process p1 = Runtime.getRuntime().exec(cmdDownload);
            
            p1.waitFor();
            
            System.out.print("[COMPLETED] : " + cmdDownload);
            
            // Run orientation
            String cmdRunOrientation = pathOrientationTool + "orientation_service " + downloadedFilename + " " + orientedFilename;
            
            System.out.print("[RUNNING] : " + cmdRunOrientation);
            
            Process p2 = Runtime.getRuntime().exec(cmdRunOrientation);

            p2.waitFor();
            
            System.out.print("[COMPLETED] : " + cmdRunOrientation);
            
            // Upload output
            String cmdUploadOutput = "python " + pathGSSTools + "upload_gss.py " + outputURI + " " + orientedFilename + " " + sessionToken;
            
            System.out.print("[RUNNING] : " + cmdUploadOutput);
            
            Process p3 = Runtime.getRuntime().exec(cmdUploadOutput);
            
            System.out.print("[COMPLETED] : " + cmdUploadOutput);
               
            annotated_STL_URI_out.value      = outputURI;
            absolute_printability_flag.value = true;
            
        }
        catch(IOException e)
        {           
            annotated_STL_URI_out.value      = e.getMessage();
            absolute_printability_flag.value = false;
        }        
        catch(InterruptedException e)
        {
            
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
