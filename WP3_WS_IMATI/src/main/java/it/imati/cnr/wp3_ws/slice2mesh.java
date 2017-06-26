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
@WebService(serviceName = "slice2mesh")
public class slice2mesh
{
 
    // The namespace should match the package name in the first non-commented line of this file. 
    // If package name is a.b.c, the namespace should be "http://c.b.a/" (casae sensitive)
    // WFM will have an easier time recognizing your web service if this is fulfilled
    //
    private final String namespace = "http://wp3_ws.cnr.imati.it/";
        
    
    @WebMethod(operationName = "slice2mesh_opname")
    public void slice2mesh_operation (
            @WebParam(name            = "serviceID", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String serviceID,
            
            @WebParam(name            = "sessionToken", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String sessionToken,
            
            @WebParam(name            = "cli_file_URI_in",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  String cil_file_URI_in,
            
            @WebParam(name            = "tetmesh_URI_out",
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<String> tetmesh_URI_out)
    {
        try
        {
            DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
            String sdate = dateFormat.format(new Date());
            
            String pathGSSTools       = "/root/infrastructureClients/gssClients/gssPythonClients/";
            String pathSlice2Mesh     = "/root/CaxMan/slice2mesh/";
            String downloadedFilename = "/root/CAxManIO/dowloaded_" + sdate + ".cli";
            String tetmeshFilename    = "/root/CAxManIO/tetmesh_" + sdate + ".vtu";
            String outputURI          = "swift://caxman/imati-ge/tetmesh_" + sdate + ".vtu";
            
            // Download File
            String cmdDownload = "python " + pathGSSTools + "download_gss.py " + cil_file_URI_in + " " + downloadedFilename + " " + sessionToken;
            
            System.out.print("[RUNNING] : " + cmdDownload);
            
            Process p1 = Runtime.getRuntime().exec(cmdDownload);
            
            p1.waitFor();   // wait the download process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdDownload);
            
            // Check if the input has been downloaded
            File input = new File(downloadedFilename);
            if (!input.getAbsoluteFile().exists()) throw new IOException("Error in downloading " + cil_file_URI_in);
            
            String cmdRunSlice2mesh = pathSlice2Mesh + "slice2mesh_caxman " + downloadedFilename + " " + tetmeshFilename;
            
            System.out.print("[RUNNING] : " + cmdRunSlice2mesh);
            
            Process p2 = Runtime.getRuntime().exec(cmdRunSlice2mesh);

            p2.waitFor();   // wait the process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdRunSlice2mesh);
            
            // Check if the output has been generated
            File output = new File(tetmeshFilename);
            if (!output.getAbsoluteFile().exists()) throw new IOException("Error in generating output " + tetmeshFilename);
            
            // Upload output
            String cmdUploadOutput = "python " + pathGSSTools + "upload_gss.py " + outputURI + " " + tetmeshFilename + " " + sessionToken;
            
            System.out.print("[RUNNING] : " + cmdUploadOutput);
            
            Process p3 = Runtime.getRuntime().exec(cmdUploadOutput);
            
            p3.waitFor();   // wait the upload process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdUploadOutput);
               
            // Return the address of the uploaded output
            tetmesh_URI_out.value        = tetmeshFilename;
            
        }
        catch(IOException e)
        {           
            tetmesh_URI_out.value        = "";
            
            System.err.println("ERROR: " + e.getMessage());
        }        
        catch(InterruptedException e)
        {
            tetmesh_URI_out.value        = "";
            
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
        Logger.getLogger(support_structures_generation.class.getName()).log(Level.SEVERE, null, ex);
    }}
