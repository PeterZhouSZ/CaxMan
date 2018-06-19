/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.imati.cnr.tools.demo;	///////////////// UPDATE WITH YOURS

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
import javax.xml.bind.DatatypeConverter;
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
    @WebMethod(operationName = "OrientationOptimizationMethod") ///////////////// UPDATE WITH YOUR NAME AND PARAMETERS
    public void orientation_optimization_async(
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
                      mode            = WebParam.Mode.OUT) Holder<String> status_base64) ///////////////// LEAVE status_base64 parameter - mandatory in CAxMan
    {
         // Keep in mind that serviceID should be stored together with the application started 
    // by the main service (startAsyncService) somehow so that getServiceStatus and 
    // abortService services are able to recognize which running instance of the application to 
    // get the status of or to kill.
    // In this example we create one "workspace" for each execution in the /tmp/ directory, 
    // where we create a folder /tmp/<serviceID>. This makes it possibly for the other two 
    // services to look into this folder to check up on the correct running application.
    
        log("Async_example.startAsyncService - started Orientation Optimization with input:" + 
                "\n\tserviceID =" + serviceID + 
                "\n\tsessionToken =" + sessionToken);
        
        
        DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
        String sdate = dateFormat.format(new Date());
        
        String outputURI = "swift://caxman/imati-ge/output_orientation_" + sdate + ".off";   ///////////////// UPDATE WITH YOURS
               
        System.out.print("[EXPECTED FINAL OUTPUT] " + outputURI);
        
        try {
            // Create a folder in tmp based on the serviceID, which will serve
            // as a local storage space for this service
            String localFolderName = "/tmp/" + serviceID;
            File temporaryFolder = new File(localFolderName);
            
            System.out.print("[CREATING FOLDER]" + localFolderName);
            
            if (temporaryFolder.exists() && !temporaryFolder.isDirectory()) {
                throw new IOException("Temporary output path exist and is not a folder");
            }
            if (!temporaryFolder.exists()) {
                temporaryFolder.mkdir();
                
                System.out.print("[CREATED FOLDER]" + localFolderName);
            }

            // This could also be an input, but is here hardcoded so that the example
            // will not grow too large.
            String outputFolderGSS = "swift://caxman/imati-ge/";
            
            String statusFileName = localFolderName + "/status.txt";
            String resultFileName = localFolderName + "/result.txt";
            String fileToUploadName = localFolderName + "/output_orientation_" + sdate + ".off";
            
            // Start the long running job - leave this as it is
            String applicationFileName = "/usr/local/bin/asyncStarter.sh";
            
            ProcessBuilder procBuilder = new ProcessBuilder(applicationFileName, sessionToken, serviceID,  
                    statusFileName, 
                    resultFileName, 
                    fileToUploadName, 
                    outputFolderGSS,	///////////////// until this parameter, leave as it is - then add parameters to your sh
                    mesh_in,
                    wq.toString(),
                    wt.toString(),
                    ws.toString(),
                    threshold.toString(),
                    ndirs.toString(),
                    outputURI);
            
            System.out.print("[STARTING APPLICATION]" + applicationFileName);
            
            Process detachedProc = procBuilder.start();
            detachedProc.waitFor();
            detachedProc.destroy();
            
            System.out.print("[DETACHED APPLICATION]" + applicationFileName);
            
  
            // The long running job is just started, so we create a GUI showing
            // a progress bar on 0%
            String html = htmlStatusBar("0");
            status_base64.value = DatatypeConverter.printBase64Binary(html.getBytes());
            
            
            // We do not know the name of the output file yet, and assign a dummy value to it.
            // If this is not done, WFM throws a null exception and your workflow fails.
            mesh_out.value = "UNSET";

        } 
        catch (IOException | InterruptedException t) 
        {   
            System.out.print("[ERROR]" + t.getMessage());
        }

    }

    
    /**
     * Web service operation
     * @param serviceID
     * @param sessionToken
     * @param mesh_out
     * @param status_base64
     */
    @WebMethod(operationName = "getServiceStatus")
    public void getServiceStatus(
            @WebParam(name = "serviceID",
                    targetNamespace = namespace, 
                    mode = WebParam.Mode.IN) String serviceID,
            @WebParam(name = "sessionToken",
                    targetNamespace = namespace,
                    mode = WebParam.Mode.IN) String sessionToken,
            @WebParam(name = "mesh_out", 
                    targetNamespace = namespace, 
                    mode = WebParam.Mode.OUT) Holder<String> mesh_out,
            @WebParam(name = "status_base64", 
                    targetNamespace = namespace, 
                    mode = WebParam.Mode.OUT) Holder<String> status_base64) 
    {
        
        log("getStatus: serviceID = " + serviceID);
        
        BufferedReader reader = null;
        String folderName = "/tmp/" + serviceID;
        String statusFileName = folderName + "/status.txt";
        String resultFileName = folderName + "/result.txt";
        
        mesh_out.value = "UNSET";
        status_base64.value = "UNSET";
        
        try {
            
            // Check old status if it exists:
            String oldStatus = "-1";
            if (new File(statusFileName).exists()) {
                oldStatus = readFile(statusFileName);
            }
        
            // Get the status file from the remote job
            //String readStatusCommand = "remoteCopying.sh " + statusFileName;           
            //Process proc = Runtime.getRuntime().exec(statusFileName);
            //proc.waitFor();
            //proc.destroy();
            
            // Set the status_base64 value according to the value of the status file.
            // We print some information to the Glassfish log for debug purposes.
            String newStatus = readFile(statusFileName);
            if ( (oldStatus.equals(newStatus)) && (!newStatus.equals("100")) ) {
                log("\nUNCHANGED\n");
                status_base64.value = "UNCHANGED";
                
                System.out.printf("STATUS:" + status_base64.value);
                System.out.printf("OLD STATUS:" + oldStatus);
                System.out.printf("NEW STATUS:" + newStatus);
                                
            }
            else if ( newStatus.equals("100") ) {
                log("\nCOMPLETED\n");
                status_base64.value = "COMPLETED";
                mesh_out.value = readFile(resultFileName);
                
                System.out.printf("STATUS:" + status_base64.value);
                System.out.printf("RESULT MESH:" + mesh_out.value);
            }
            else {
                log("\nNeither unchanged nor completed, but:\n" + newStatus);
                status_base64.value = newStatus;
                
                System.out.printf("STATUS:" + status_base64.value);
            }
            
            String html = htmlStatusBar(newStatus);
            status_base64.value = DatatypeConverter.printBase64Binary(html.getBytes());
        
        } 
        catch (IOException ex) 
        {
            error(ex);
            error("null:fileStatus=" + statusFileName);
            
            status_base64.value = "0";
            
            System.out.printf("ERROR:" + ex.getMessage());
        }
        finally 
        {
            try 
            {
                if (reader != null) 
                {
                    reader.close();
                }
            } catch (IOException ex) 
            {              
                error(ex);
            }
        }
        
        // If the status is updated and the job is not yet finished, we create
        // a new progress bar based on the status file.
        //if ( !status_base64.value.equals("UNCHANGED") && !status_base64.value.equals("COMPLETED") ) {
//            String html = htmlStatusBar(newStatus);
//            status_base64.value = DatatypeConverter.printBase64Binary(html.getBytes());
        //}
    }
//    
//    
    /**
     * Web service operation
     * @param serviceID
     * @param sessionToken
     * @param result
     */
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
            //"<link href=\"https://api.eu-cloudflow.eu/portal/twopointo/styles/style.css\" rel=\"stylesheet\" type=\"text/css\">\n" +
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
