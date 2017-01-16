/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.imati.cnr.wp3_ws;

import java.io.File;
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
            
            @WebParam(name            = "annotated_tessellation_URI_in",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  String annotated_tessellation_URI_in,
            
            @WebParam(name            = "annotated_STL_URI_out", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<String> annotated_STL_URI_out,
            
            @WebParam(name            = "absolute_printability_flag", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<Integer> absolute_printability_flag) 
    {        
        try
        {
            DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
            String sdate = dateFormat.format(new Date());
            
            String pathGSSTools         = "/root/infrastructureClients/gssClients/gssPythonClients/";
            String pathOrientationTool  = "/root/CaxMan/orientation_service/";
            String downloadedFilename   = "/root/CAxManIO/dowloaded_" + sdate + ".zip";      
            String orientedFilename     = "/root/CAxManIO/oriented_" + sdate + ".zip";
            String outputURI            = "swift://caxman/imati-ge/oriented_" + sdate + ".zip";
            
            // Download File
            String cmdDownload = "python " + pathGSSTools + "download_gss.py " + annotated_tessellation_URI_in + " " + downloadedFilename + " " + sessionToken;
            
            System.out.print("[RUNNING] : " + cmdDownload);
            
            Process p1 = Runtime.getRuntime().exec(cmdDownload);
            
            p1.waitFor();   // wait the download process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdDownload);
            
            // Check if the input has been downloaded
            File input = new File(downloadedFilename);
            if (!input.getAbsoluteFile().exists()) throw new IOException("Error in downloading " + annotated_tessellation_URI_in);
            
            // Run orientation
            String cmdRunOrientation = pathOrientationTool + "orientation_service " + downloadedFilename + " " + orientedFilename;
            
            System.out.print("[RUNNING] : " + cmdRunOrientation);
            
            Process p2 = Runtime.getRuntime().exec(cmdRunOrientation);

            p2.waitFor();   // wait the orientation process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdRunOrientation);
            
            // Check if the output has been generated
            File output = new File(orientedFilename);
            if (!output.getAbsoluteFile().exists()) throw new IOException("Error in generating output " + orientedFilename);
            
            // Upload output
            String cmdUploadOutput = "python " + pathGSSTools + "upload_gss.py " + outputURI + " " + orientedFilename + " " + sessionToken;
            
            System.out.print("[RUNNING] : " + cmdUploadOutput);
            
            Process p3 = Runtime.getRuntime().exec(cmdUploadOutput);
            
            p3.waitFor();   // wait the upload process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdUploadOutput);
               
            // Return the address of the uploaded output
            //annotated_STL_URI_out.value      = outputURI;
            annotated_STL_URI_out.value      = orientedFilename;
            absolute_printability_flag.value = 0;
            
            // Remove input and output files
            //input.delete();
            //output.delete();
            
        }
        catch(IOException e)
        {           
            annotated_STL_URI_out.value      = "";
            absolute_printability_flag.value = 1;
            
            System.err.println("ERROR: " + e.getMessage());
        }        
        catch(InterruptedException e)
        {
            annotated_STL_URI_out.value      = "";
            absolute_printability_flag.value = 1;
            
            System.err.println("ERROR: " + e.getMessage());
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
