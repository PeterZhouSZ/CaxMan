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
@WebService(serviceName = "OrientationOptimizationService")
public class OrientationOptimization 
{
    private final String namespace = "http://wp3_ws.cnr.imati.it/";

    /**
     * Web service operation
     * @param serviceID
     * @param sessionToken
     * @param mesh_in
     * @param wq
     * @param wt
     * @param mesh_out
     * @param ws
     * @param threshold
     * @param ndirs
     * @return 
     */
    @WebMethod(operationName = "OrientationOptimizationMethod")
    public String orientation_optimization(
            @WebParam(name            = "serviceID", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String serviceID,
            
            @WebParam(name            = "sessionToken", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String sessionToken,
            
            @WebParam(name            = "mesh_in",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  String mesh_in,
            
             @WebParam(name            = "wq",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  Double wq,
             
              @WebParam(name            = "wt",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  Double wt,
              
               @WebParam(name            = "ws",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  Double ws,
               
                @WebParam(name            = "threshold",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  Double threshold ,
                
                 @WebParam(name            = "ndirs",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  Integer ndirs,
            
            @WebParam(name            = "mesh_out", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<String> mesh_out)
    {
        DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
        String sdate = dateFormat.format(new Date());
            
        String pathGSSTools         = "/root/infrastructureClients/gssClients/gssPythonClients/";
        String pathTool             = "/root/CaxMan/demo_services/orientation_optimization/build/";
        String downloadedFilename   = "/root/CAxManIO/dowloaded_" + sdate + ".off";      
        String outputFilename       = "/root/CAxManIO/output_orientation" + sdate + ".off";
        String outputURI            = "swift://caxman/imati-ge/output_orientation" + sdate + ".off";
        
        String cmdDownload = "python " + pathGSSTools + "download_gss.py " + mesh_in + " " + downloadedFilename + " " + sessionToken;
        
        String cmdRunOperation = pathTool + "orient_service " + downloadedFilename + " " + outputFilename + 
                                                            " " + wq + " " + wt + " " + ws + " " + threshold + " " + ndirs;
        
        String cmdUploadOutput = "python " + pathGSSTools + "upload_gss.py " + outputURI + " " + outputFilename + " " + sessionToken;
        
        //executable usage : orientatio_service inmesh outmesh wq wt ws [thresh=30] [ndirs=500]"
        
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
            mesh_out.value      = outputFilename;
            
        }
        catch(IOException | InterruptedException e)
        {   
            System.err.println("ERROR: " + e.getMessage());
        }
        
        
        return null;
    }
    
}
