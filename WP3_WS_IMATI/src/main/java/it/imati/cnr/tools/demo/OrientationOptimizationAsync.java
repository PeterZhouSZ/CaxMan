/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.imati.cnr.tools.demo;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import javax.xml.ws.Holder;

/**
 *
 * @author daniela
 */
@WebService(serviceName = "OrientationOptimizationAsyncService")
public class OrientationOptimizationAsync 
{
    private final String namespace = "http://demo.tools.cnr.imati.it/";

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
     * @param status_base64
     */
    @WebMethod(operationName = "OrientationOptimizationMethod")
    public void orientation_optimization(
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
                      mode            = WebParam.Mode.OUT)  Holder<String> mesh_out,
            
            @WebParam(name            = "status_base64", 
                      targetNamespace = namespace, 
                      mode            = WebParam.Mode.OUT) Holder<String> status_base64)
    {
        try {
            DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
            String sdate = dateFormat.format(new Date());
            
           
            String tool             = "/root/CaxMan/demo_services/scripts/orientation_optimization.sh";
            String outputURI            = "swift://caxman/imati-ge/output_orientation_" + sdate + ".off";
            
            
            String cmdRunOperation = tool + " " + sessionToken + " " + mesh_in + " " + outputURI +
                    " " + wq + " " + wt + " " + ws + " " + threshold + " " + ndirs;
                        
            //executable usage : orientation_service inmesh outmesh wq wt ws [thresh=30] [ndirs=500]"
            
            mesh_out.value = "";
            
            System.out.print("[RUNNING] : " + cmdRunOperation);
            Process p2 = Runtime.getRuntime().exec(cmdRunOperation);
                    
            p2.waitFor();
            p2.destroy();
            
        } 
        catch (IOException | InterruptedException ex) 
        {
            Logger.getLogger(OrientationOptimizationAsync.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    @WebMethod(operationName = "getServiceStatus")
    public void getServiceStatus(
            @WebParam(name = "serviceID",
                    targetNamespace = namespace, 
                    mode = WebParam.Mode.IN) String serviceID,
            @WebParam(name = "sessionToken",
                    targetNamespace = namespace,
                    mode = WebParam.Mode.IN) String sessionToken,
            @WebParam(name = "outputFile", 
                    targetNamespace = namespace, 
                    mode = WebParam.Mode.OUT) Holder<String> outputFile,
            @WebParam(name = "status_base64", 
                    targetNamespace = namespace, 
                    mode = WebParam.Mode.OUT) Holder<String> status_base64) 
    {
        
        log("getStatus: serviceID = " + serviceID);
        BufferedReader reader = null;
        String folderName = "/tmp/" + serviceID;
        String statusFileName = folderName + "/status.txt";
        String resultFileName = folderName + "/result.txt";
    }
    
    
    @WebMethod(operationName = "abortService")
    public void abortService(
            @WebParam(name = "serviceID",
                    targetNamespace = namespace, 
                    mode = WebParam.Mode.IN) String serviceID,
            @WebParam(name = "sessionToken",
                    targetNamespace = namespace,
                    mode = WebParam.Mode.IN) String sessionToken,
            @WebParam(name = "result",
                    targetNamespace = namespace,
                    mode = WebParam.Mode.OUT) Holder<Boolean> result)
    {
        // Here you should implement functionality to kill/abort the job started by startAsyncService
        // based on the unique serviceID.
        
        // The "result" parameter should be true if the job was succesfully aborted,
        // and false if the opposite.
        
        // Since this implementation does nothing, we return false:
        result.value = false;
    }
    
    
    
    /*
    *  Utility function for HTML progress bar
    */
    private String htmlStatusBar(String progressAsString) {
        int progress = new Integer(progressAsString);
        int maxWidth = 800;

        int relativeProgress = (int)((progress/100.0 ) * maxWidth);

        String html = "<html>\n" +
            "<head>\n" +
            "<title>blah</title>\n" +
            "<link href=\"https://api.eu-cloudflow.eu/portal/twopointo/styles/style.css\" rel=\"stylesheet\" type=\"text/css\">\n" +
            "</head>\n" +
            "<body style=\"margin: 20px; padding: 20px;\">\n" +
            "<h1>Doing automatic registration</h1>\n" +
            "<div style=\"border-radius: 5px; border-color: lightblueblue; border-style:dashed; width: " + maxWidth + "px; height: 80px;padding:0; margin: 0; border-width: 3px;\">\n" +
            "<div style=\"position: relative; top: -3px; left: -3px; border-radius: 5px; border-color: lightblue; border-style:solid; width: " + relativeProgress + "px; height: 80px;padding:0; margin: 0; border-width: 3px; background-color: lightblue;\">\n" +
            "<h1 style=\"margin-left: 20px;\" >" + progress + "%</h1>\n" +
            "</div>\n" +
            "</div>\n" +
            "</head>\n" +
            "</body>";
        
        return html;
    }
    
    /*
    *  Utility function for reading a file
    */
    private String readFile(String fileName) throws FileNotFoundException, IOException {
        BufferedReader reader = new BufferedReader(new FileReader(fileName));
        String msg;
        if ( (msg = reader.readLine()) == null ) {
            throw new IOException("Registration::getStatus: No content in " + fileName);
        }   
        return msg;
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
        Logger.getLogger(OrientationOptimizationAsync.class.getName()).log(Level.SEVERE, null, ex);
    }
}
