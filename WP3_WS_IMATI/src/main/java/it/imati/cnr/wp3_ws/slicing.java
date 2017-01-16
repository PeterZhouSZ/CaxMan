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
@WebService(serviceName = "slicing")
public class slicing 
{
 
    // The namespace should match the package name in the first non-commented line of this file. 
    // If package name is a.b.c, the namespace should be "http://c.b.a/" (casae sensitive)
    // WFM will have an easier time recognizing your web service if this is fulfilled
    //
    private final String namespace = "http://wp3_ws.cnr.imati.it/";
        
    
    @WebMethod(operationName = "slicing_opname")
    public void slicing_operation (
            @WebParam(name            = "serviceID", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String serviceID,
            
            @WebParam(name            = "sessionToken", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String sessionToken,
            
            @WebParam(name            = "annotated_tessellation_URI_in",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  String annotated_tessellation_URI_in,
            
            @WebParam(name            = "sliced_CLI_URI_out", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<String> sliced_CLI_URI_out,
            
            @WebParam(name            = "volumetric_CLI_URI_out", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<String> volumetric_CLI_URI_out) 
    {
        try
        {
            DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
            String sdate = dateFormat.format(new Date());
            
            String pathGSSTools         = "/root/infrastructureClients/gssClients/gssPythonClients/";
            String pathOrientationTool  = "/root/stl2cli/build/";
            String downloadedFilename   = "/root/CAxManIO/dowloaded_" + sdate + ".zip";      
            String slicedFilename       = "/root/CAxManIO/sliced_" + sdate + ".zip";
            String outputURI            = "swift://caxman/imati-ge/sliced_" + sdate + ".zip";
            
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
            String cmdRunOrientation = pathOrientationTool + "stl2cli " + downloadedFilename + " " + slicedFilename;
            
            System.out.print("[RUNNING] : " + cmdRunOrientation);
            
            Process p2 = Runtime.getRuntime().exec(cmdRunOrientation);

            p2.waitFor();   // wait the orientation process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdRunOrientation);
            
            // Check if the output has been generated
            File output = new File(slicedFilename);
            if (!output.getAbsoluteFile().exists()) throw new IOException("Error in generating output " + slicedFilename);
            
            // Upload output
            String cmdUploadOutput = "python " + pathGSSTools + "upload_gss.py " + outputURI + " " + slicedFilename + " " + sessionToken;
            
            System.out.print("[RUNNING] : " + cmdUploadOutput);
            
            Process p3 = Runtime.getRuntime().exec(cmdUploadOutput);
            
            p3.waitFor();   // wait the upload process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdUploadOutput);
               
            // Return the address of the uploaded output
            sliced_CLI_URI_out.value        = slicedFilename;            
            volumetric_CLI_URI_out.value    = "";
            
        }
        catch(IOException e)
        {           
            sliced_CLI_URI_out.value        = "";
            volumetric_CLI_URI_out.value    = "";
            
            System.err.println("ERROR: " + e.getMessage());
        }        
        catch(InterruptedException e)
        {
            sliced_CLI_URI_out.value        = "";
            volumetric_CLI_URI_out.value    = "";
            
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
        Logger.getLogger(slicing.class.getName()).log(Level.SEVERE, null, ex);
    }}
