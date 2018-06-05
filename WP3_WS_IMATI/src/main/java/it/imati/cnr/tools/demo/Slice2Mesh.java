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
@WebService(serviceName = "SliceToMeshService")
public class Slice2Mesh 
{
    private final String namespace = "http://demo.tools.cnr.imati.it/";

    /**
     * Web service operation
     * @param serviceID
     * @param sessionToken
     * @param mesh_in
     * @param hatch_thickness
     * @param surface_out
     * @param volume_out
     * @param mesh_out
     * @return 
     */
    @WebMethod(operationName = "SliceToMeshMethod")
    public String slice_to_mesh(
            @WebParam(name            = "serviceID", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String serviceID,
            
            @WebParam(name            = "sessionToken", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.IN)  String sessionToken,
            
            @WebParam(name            = "mesh_in",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  String mesh_in,
            
            @WebParam(name            = "hatch_thickness",
                      targetNamespace = namespace,
                      mode            = WebParam.Mode.IN)  Double hatch_thickness,
            
            @WebParam(name            = "surface_out", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<String> surface_out,
            
            @WebParam(name            = "volume_out", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT)  Holder<String> volume_out) 
    {
        
        if (hatch_thickness == null)
            hatch_thickness = 0.0;
        
        DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
        String sdate = dateFormat.format(new Date());
            
        String pathGSSTools         = "/root/infrastructureClients/gssClients/gssPythonClients/";
        String pathTool             = "/root/CaxMan/demo_services/slice_2_mesh/";
        String downloadedFilename   = "/root/CAxManIO/dowloaded_" + sdate + ".off";      
        String outputSurfFilename       = "/root/CAxManIO/output_surf_mesh" + sdate + ".off";
        String outputVolFilename       = "/root/CAxManIO/output_volume_mesh" + sdate + ".mesh";
        String outputURI            = "swift://caxman/imati-ge/output_volume_mesh" + sdate + ".off";
        
        String cmdDownload = "python " + pathGSSTools + "download_gss.py " + mesh_in + " " + downloadedFilename + " " + sessionToken;
        String cmdRunOperation = pathTool + "slice2mesh_service "   + downloadedFilename + " " 
                                                                    + outputSurfFilename + " " 
                                                                    + outputVolFilename + " " + hatch_thickness;
        
        
        String cmdUploadOutputSurf = "python " + pathGSSTools + "upload_gss.py " + outputURI + " " + outputSurfFilename + " " + sessionToken;
        String cmdUploadOutputVol = "python " + pathGSSTools + "upload_gss.py " + outputURI + " " + outputVolFilename + " " + sessionToken;

        
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
            File surf_output = new File(outputSurfFilename);
            if (!surf_output.getAbsoluteFile().exists()) throw new IOException("Error in generating output " + outputSurfFilename);
            
            File vol_output = new File(outputVolFilename);
            if (!vol_output.getAbsoluteFile().exists()) throw new IOException("Error in generating output " + outputVolFilename);
            
            //##########################################################################################################
            // Upload output surface
            System.out.print("[RUNNING] : " + cmdUploadOutputSurf);
            
            Process p3 = Runtime.getRuntime().exec(cmdUploadOutputSurf);
            
            p3.waitFor();   // wait the upload process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdUploadOutputSurf);
            
            // Upload output volume
            System.out.print("[RUNNING] : " + cmdUploadOutputVol);
            
            Process p4 = Runtime.getRuntime().exec(cmdUploadOutputVol);
            
            p4.waitFor();   // wait the upload process to finish its task
            
            System.out.print("[COMPLETED] : " + cmdUploadOutputVol);
            
            
               
            //##########################################################################################################
            // Return the address of the uploaded output
            volume_out.value  = outputVolFilename;
            surface_out.value = outputSurfFilename;
            
        }
        catch(IOException | InterruptedException e)
        {   
            System.err.println("ERROR: " + e.getMessage());
        }
        
        return null;
    }
    
}
