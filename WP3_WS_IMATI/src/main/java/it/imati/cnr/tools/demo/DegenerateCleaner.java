/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.imati.cnr.tools.demo;

import java.io.File;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import javax.xml.ws.Holder;

/**
 *
 * @author daniela
 */
@WebService(serviceName = "DegenerateCleanerService")
public class DegenerateCleaner 
{   
    private final String namespace = "http://demo.tools.cnr.imati.it/";


    /**
     * Web service operation
     * @param serviceID
     * @param sessionToken
     * @param mesh_in
     * @param mesh_out
     */
    @WebMethod(operationName = "DegenerateCleanerMethod")
    public void degenerate_cleaner(
            @WebParam(name            = "serviceID", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String serviceID,
            
            @WebParam(name            = "sessionToken", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String sessionToken,
            
            @WebParam(name            = "mesh_in",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  String mesh_in,
            
            @WebParam(name            = "mesh_out", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<String> mesh_out)
    {
        
        System.out.print("[ INPUT MESH ] : " + mesh_in);

        
        
        DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
        String sdate = dateFormat.format(new Date());
            
        String pathGSSTools         = "/root/infrastructureClients/gssClients/gssPythonClients/";
        String pathTool             = "/root/CaxMan/demo_services/degenerate_cleaner/build/";
        String downloadedFilename   = "/root/CAxManIO/dowloaded_" + sdate + ".off";      
        String outputFilename       = "/root/CAxManIO/output_deg_cleaner_" + sdate + ".off";
        String outputURI            = "swift://caxman/imati-ge/output_deg_cleaner_" + sdate + ".off";
        
        String cmdDownload = "python " + pathGSSTools + "download_gss.py " + mesh_in + " " + downloadedFilename + " " + sessionToken;
        String cmdRunOperation = pathTool + "deg_cleaner " + downloadedFilename + " " + outputFilename;
        String cmdUploadOutput = "python " + pathGSSTools + "upload_gss.py " + outputURI + " " + outputFilename + " " + sessionToken;
        
        if (mesh_in == null)
        {
            mesh_out.value = "NO INPUT.";
            return;
        }
        
        
        try
        {
            //##########################################################################################################
            // Download File
            System.out.print("[RUNNING] : " + cmdDownload);
            
            Process p1 = Runtime.getRuntime().exec(cmdDownload);
            
            p1.waitFor();   // wait the download process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdDownload);
            
            // Check if the input has been downloaded
            File input = new File(downloadedFilename);
            if (!input.getAbsoluteFile().exists()) throw new IOException("Error in downloading " + mesh_in);
            
            //##########################################################################################################
            // Run 
            System.out.print("[RUNNING] : " + cmdRunOperation);
            
            Process p2 = Runtime.getRuntime().exec(cmdRunOperation);

            p2.waitFor();   // wait the orientation process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdRunOperation);
            
            // Check if the output has been generated
            File output = new File(outputFilename);
            if (!output.getAbsoluteFile().exists()) throw new IOException("Error in generating output " + outputFilename);
            
            //##########################################################################################################
            // Upload output
            System.out.print("[RUNNING] : " + cmdUploadOutput);
            
            Process p3 = Runtime.getRuntime().exec(cmdUploadOutput);
            
            p3.waitFor();   // wait the upload process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdUploadOutput);
               
            //##########################################################################################################
            // Return the address of the uploaded output
            mesh_out.value      = outputURI;
            
        }
        catch(IOException | InterruptedException e)
        {   
            System.err.println("ERROR: " + e.getMessage());
        }
    }
    
}
